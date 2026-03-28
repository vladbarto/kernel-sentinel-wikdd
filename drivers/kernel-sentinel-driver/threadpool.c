//
//   Copyright (C) 2019-2026 BitDefender S.R.L.
//   Author(s)    : Andrei-Marius MUNTEA(amuntea@bitdefender.com)
//                : Radu PORTASE(rportase@bitdefender.com)
//

#include "include\threadpool.h"

/// Introduce a work routine (mirroring UM TestThreadPoolRoutine at the moment)
NTSTATUS
MyWorkItemRoutine(
    _In_opt_ PVOID Context
) {
    PMY_CONTEXT ctx = (PMY_CONTEXT)Context;

    if (NULL == ctx)
    {
        return STATUS_INVALID_PARAMETER;
    }

    KIRQL OldIrql = ctx->Irql;

    for (UINT32 i = 0; i < 1000; ++i)
    {
        KeAcquireSpinLock(&ctx->SpinLock, &OldIrql);
        ctx->Number++;
        DbgPrintEx(
            DPFLTR_IHVDRIVER_ID,
            DPFLTR_ERROR_LEVEL,
            "Hello from My Work Item Routine! Number = %d\r\n",
            ctx->Number
        ); 
        KeReleaseSpinLock(&ctx->SpinLock, OldIrql);
    }

    return STATUS_SUCCESS;
}


/// See https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/wdm/nc-wdm-kstart_routine
_IRQL_requires_same_
_Function_class_(KSTART_ROUTINE)
VOID
TpWorkerThread(
    _In_ PVOID StartContext
)
{
    PMY_THREAD_POOL threadPool = (PMY_THREAD_POOL)(StartContext);
    PVOID objects[2] = { 0 };
    objects[0] = &threadPool->StopThreadPool;
    objects[1] = &threadPool->WorkScheduled;

    BOOLEAN executing = TRUE;
    while (executing)
    {
        // Wait for any of the thread pool events.
        BOOLEAN shouldWork = FALSE;
        const NTSTATUS status = KeWaitForMultipleObjects(ARRAYSIZE(objects),
            objects,
            WaitAny,
            Executive,
            KernelMode,
            FALSE,
            NULL,
            NULL);
        switch (status)
        {
            case STATUS_WAIT_0 + 1:     // The work event was scheduled.
                shouldWork = TRUE;
                break;
            case STATUS_WAIT_0:         // The termination event was scheduled.
            default:                    // Wait failed (we also stop).
                executing = FALSE;
                continue;
                break;
        }

        while (shouldWork)
        {
            KeWaitForSingleObject(&threadPool->ListMutex,
                Executive,
                KernelMode,
                FALSE,
                NULL);
            if (!IsListEmpty(&threadPool->ListHead))
            {
                LIST_ENTRY* entry = RemoveTailList(&threadPool->ListHead);
                MY_WORK_ITEM* workItem = CONTAINING_RECORD(entry, MY_WORK_ITEM, ListEntry);

                workItem->Routine(workItem->Context);
                ExFreePoolWithTag(workItem, 'KMSD');
            }
            else
            {
                shouldWork = FALSE;
            }
            KeReleaseMutex(&threadPool->ListMutex, FALSE);
        }
    }
}

NTSTATUS
TpUninitialize(
    _Pre_valid_ _Post_invalid_ PMY_THREAD_POOL ThreadPool
)
{
    if (!ThreadPool) {
        /* We can just stop, nothing to do */
        return STATUS_NOTHING_TO_TERMINATE;
    }

    // Signal the stop events so that all threads stop.
    KeSetEvent(&ThreadPool->StopThreadPool, 0, FALSE);

    // Wait for all threads to terminate.
    for (UINT32 i = 0; i < ThreadPool->NumberOfThreads; i++)
    {
        if (NULL != ThreadPool->ThreadHandles[i])
        {
            ZwWaitForSingleObject(ThreadPool->ThreadHandles[i], FALSE, NULL);
            ZwClose(ThreadPool->ThreadHandles[i]);
            ThreadPool->ThreadHandles[i] = NULL;
        }
    }
    if (ThreadPool->ThreadHandles)
    {
        ExFreePoolWithTag(ThreadPool->ThreadHandles, 'KMSD');
        ThreadPool->ThreadHandles = NULL;
    }

    //
    // Flush the remaining work items.
    // TODO: Proper locking  :)
    //
    while (!IsListEmpty(&ThreadPool->ListHead))
    {
        LIST_ENTRY* entry = RemoveHeadList(&ThreadPool->ListHead);
        MY_WORK_ITEM* workItem = CONTAINING_RECORD(entry, MY_WORK_ITEM, ListEntry);

        workItem->Routine(workItem->Context);
        ExFreePoolWithTag(workItem, 'KMSD');
    }

    return STATUS_SUCCESS;
}

NTSTATUS
TpInit(
    _In_ PMY_THREAD_POOL ThreadPool,
    _In_ UINT32          NumberOfThreads
)
{
    if (!NumberOfThreads)
    {
        return STATUS_INVALID_PARAMETER_2;
    }

    //
    // Initialize the work item list
    //
    InitializeListHead(&ThreadPool->ListHead);
    KeInitializeMutex(&ThreadPool->ListMutex, 0);

    //
    // Initialize the events
    //

    // the stop thread pool event does not to be reset on each wakeup
    KeInitializeEvent(&ThreadPool->StopThreadPool, NotificationEvent, FALSE);

    // the work scheduled event is a synchronization event to be auto reseted
    // when a thread is woken up to perform work on the work item
    KeInitializeEvent(&ThreadPool->WorkScheduled, SynchronizationEvent, FALSE);


    //
    // Start the threads
    //

    // Allocate space to store the handles. See https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/wdm/nf-wdm-exallocatepool2
    ThreadPool->NumberOfThreads = NumberOfThreads;
    ThreadPool->ThreadHandles = ExAllocatePool2(POOL_FLAG_NON_PAGED,
        NumberOfThreads * sizeof(HANDLE),
        'KMSD');
    if (!ThreadPool->ThreadHandles)
    {
        return STATUS_INSUFFICIENT_RESOURCES;
    }
    RtlZeroMemory(ThreadPool->ThreadHandles,
        NumberOfThreads * sizeof(HANDLE));

    // Create the threads
    for (UINT32 i = 0; i < NumberOfThreads; i++)
    {
        // See https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/wdm/nf-wdm-pscreatesystemthread
        HANDLE hThread = NULL;
        const NTSTATUS status = PsCreateSystemThread(&hThread,
            0,
            NULL,
            NULL,
            NULL,
            TpWorkerThread,
            ThreadPool);
        if (!NT_SUCCESS(status))
        {
            TpUninitialize(ThreadPool);
            return status;

        }
        else
        {
            ThreadPool->ThreadHandles[i] = hThread;
        }
    }

    return STATUS_SUCCESS;
}

NTSTATUS
TpEnqueueWorkItem(
    _In_ PMY_THREAD_POOL ThreadPool,
    _In_ PKSTART_ROUTINE StartRoutine,
    _Inout_opt_ PVOID    Context
)
{
    // Allocate a new work item and initialize it
    MY_WORK_ITEM* workItem = ExAllocatePool2(POOL_FLAG_NON_PAGED,
        sizeof(MY_WORK_ITEM),
        'KMSD');
    if (!workItem)
    {
        return STATUS_INSUFFICIENT_RESOURCES;
    }
    workItem->Routine = StartRoutine;
    workItem->Context = Context;

    // Insert the work item in the list
    KeWaitForSingleObject(&ThreadPool->ListMutex, Executive, KernelMode, FALSE, NULL);
    InsertHeadList(&ThreadPool->ListHead, &workItem->ListEntry);
    KeReleaseMutex(&ThreadPool->ListMutex, FALSE);

    // Signal the work scheduled event
    KeSetEvent(&ThreadPool->WorkScheduled, 0, FALSE);
    return STATUS_SUCCESS;
}
