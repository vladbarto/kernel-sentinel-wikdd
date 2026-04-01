#pragma once
#include <ntifs.h>


//
// MY_THREAD_POOL - simple thread pool structure.
//
typedef struct _MY_THREAD_POOL
{
    KEVENT     StopThreadPool;  // When this event is signaled the thread pool should stop.
    KEVENT     WorkScheduled;   // This event is used to signal that the threads have work to perform.
    UINT32     NumberOfThreads; // Number of threads in threadpool.
    HANDLE* ThreadHandles;   // Handle to the actual threads.
    KMUTEX     ListMutex;       // We use KMUTEX to protect the list of work items
    // See https://learn.microsoft.com/en-us/windows-hardware/drivers/kernel/introduction-to-mutex-objects
    LIST_ENTRY ListHead;        // The work items pended.
    // See https://learn.microsoft.com/en-us/windows-hardware/drivers/kernel/singly-and-doubly-linked-lists#doubly-linked-lists
}MY_THREAD_POOL, * PMY_THREAD_POOL;

//
// MY_WORK_ITEM - A very basic generic work item.
//
typedef struct _MY_WORK_ITEM
{
    LIST_ENTRY      ListEntry;
    PKSTART_ROUTINE Routine;
    PVOID           Context;
}MY_WORK_ITEM, * PMY_WORK_ITEM;


_IRQL_requires_same_
_Function_class_(KSTART_ROUTINE)
VOID
TpWorkerThread(
    _In_ PVOID StartContext
);

VOID
TpUninitialize(
    _Pre_valid_ _Post_invalid_ PMY_THREAD_POOL ThreadPool
);

NTSTATUS
TpInit(
    _In_ PMY_THREAD_POOL ThreadPool,
    _In_ UINT32          NumberOfThreads
);

NTSTATUS
TpEnqueueWorkItem(
    _In_ PMY_THREAD_POOL ThreadPool,
    _In_ PKSTART_ROUTINE StartRoutine,
    _Inout_opt_ PVOID    Context
);
