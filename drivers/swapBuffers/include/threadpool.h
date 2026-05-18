#pragma once

#include "communication_protocol.h"
#include "communication.h"
#include <ntstrsafe.h>

#define MAX_NUMBER_THREADS 5
//
// MY_THREAD_POOL - simple thread pool structure.
//
typedef struct _MY_THREAD_POOL
{
    KEVENT     StopThreadPool;  // When this event is signaled the thread pool should stop.
    KEVENT     WorkScheduled;   // This event is used to signal that the threads have work to perform.
    UINT32     NumberOfThreads; // Number of threads in threadpool.
    HANDLE*    ThreadHandles;   // Handle to the actual threads.
    KSPIN_LOCK ListSpinLock;       // We use KSPIN_LOCK to protect the list of work items
    LIST_ENTRY ListHead;        // The work items pended.
    // See https://learn.microsoft.com/en-us/windows-hardware/drivers/kernel/singly-and-doubly-linked-lists#doubly-linked-lists
    KMUTEX     KMutex;
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

/* Trying to maintain here the backward compatibility with the Lab 6 implementation */
typedef struct _MY_CONTEXT
{
    KSPIN_LOCK SpinLock;
    KIRQL Irql;
    
    // Specific fields for our Minifilter notification feature
    UINT32 NotificationType;
    UINT64 ProcessId;
    UNICODE_STRING ProcessName;
    NTSTATUS OperationResult;
    union
    {
        struct
        {
            UNICODE_STRING TargetPath;
            WCHAR Details[256];
        } MiniFilter;

        struct
        {
            UINT16 LayerId;
            BOOLEAN IsOutbound; // TRUE = AUTH_CONNECT; FALSE = AUTH_RECV_ACCEPT
            BOOLEAN IsIPv6;
            UINT8 Protocol;
            UINT16 LocalPort;
            UINT16 RemotePort;
            UINT8 IcmpType;
            union
            {
                struct
                {
                    UINT32 LocalAddress;
                    UINT32 RemoteAddress;
                } Ipv4;
                struct
                {
                    UINT8 LocalAddress[16]; // IPv6 (16 bytes)
                    UINT8 RemoteAddress[16];
                } Ipv6;
            };
        } Network;
    } Data;
} MY_CONTEXT, * PMY_CONTEXT;


NTSTATUS
MyWorkItemRoutine(
    _In_ PVOID Context,
    _In_ KMUTEX TPMutex
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
