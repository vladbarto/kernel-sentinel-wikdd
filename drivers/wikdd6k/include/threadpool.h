#pragma once
#include <ntifs.h>
#include <ntstrsafe.h>
#include "communication.h"
#include "communication_protocol.h"

#define MAX_NUMBER_THREADS 5
//
// MY_THREAD_POOL - simple thread pool structure.
//
typedef struct _MY_THREAD_POOL
{
    KEVENT     StopThreadPool;  // When this event is signaled the thread pool should stop.
    KEVENT     WorkScheduled;   // This event is used to signal that the threads have work to perform.
    UINT32     NumberOfThreads; // Number of threads in threadpool.
    HANDLE* ThreadHandles;   // Handle to the actual threads.
    KSPIN_LOCK     ListSpinLock;       // We use KSPIN_LOCK to protect the list of work items
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

typedef struct _MY_CONTEXT
{
    KSPIN_LOCK SpinLock;
    KIRQL Irql;
    
    // Specific fields for our Minifilter notification feature
    UINT32 NotificationType;
    UINT32 ProcessId;
    UNICODE_STRING ProcessName;
    UNICODE_STRING TargetPath;
    NTSTATUS OperationResult;
    WCHAR Details[256];
} MY_CONTEXT, * PMY_CONTEXT;


NTSTATUS
MyWorkItemRoutine(
    _In_opt_ PVOID Context
);

_IRQL_requires_same_
_Function_class_(KSTART_ROUTINE)
VOID
TpWorkerThread(
    _In_ PVOID StartContext
);

NTSTATUS
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
