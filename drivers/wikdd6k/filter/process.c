#include "..\include\filter\process.h"
#include "..\include\filter\utils.h"
#include "..\include\communication.h"
#include "process.tmh"

void
ProcFltSendMessageProcessCreate(
    _In_ HANDLE ProcessId,
    _In_ PPS_CREATE_NOTIFY_INFO CreateInfo
)
{
    UNICODE_STRING message;
    ULONG32 msgSize = 4 * PAGE_SIZE;
    message.Buffer = ExAllocatePoolWithTag(PagedPool, msgSize, UTILS_TAG_UNICODE_STRING);
    if (!message.Buffer)
    {
        return;
    }
    message.MaximumLength = 4 * PAGE_SIZE;
    message.Length = 0;

    LARGE_INTEGER timestamp = { 0 };
    KeQuerySystemTime(&timestamp);

    RtlUnicodeStringPrintf(&message, L"[%llu] [ProcessCreate] [%u] Path = %wZ, CommandLine = %wZ",
        timestamp.QuadPart, HandleToUlong(ProcessId), CreateInfo->ImageFileName, CreateInfo->CommandLine);


    CommSendString(&message);
    ExFreePoolWithTag(message.Buffer, UTILS_TAG_UNICODE_STRING);
}

static VOID
ProcFltSendMessageProcessTerminate(
    _In_ HANDLE ProcessId
)
{
    UNICODE_STRING message;
    ULONG32 msgSize = 4 * PAGE_SIZE;
    message.Buffer = ExAllocatePoolWithTag(PagedPool, msgSize, UTILS_TAG_UNICODE_STRING);
    if (!message.Buffer)
    {
        return;
    }
    message.MaximumLength = 4 * PAGE_SIZE;
    message.Length = 0;

    LARGE_INTEGER timestamp = { 0 };
    KeQuerySystemTime(&timestamp);

    PUNICODE_STRING ImageFileName = NULL;
    NTSTATUS status = GetImagePathFromPid(ProcessId, &ImageFileName);
    if (NT_SUCCESS(status))
    {
        RtlUnicodeStringPrintf(&message, L"[%llu] [ProcessTerminate] [%u] Path = %wZ",
            timestamp.QuadPart, HandleToUlong(ProcessId), ImageFileName);

        ExFreePoolWithTag(ImageFileName, UTILS_TAG_UNICODE_STRING);
    }
    else
    {
        RtlUnicodeStringPrintf(&message, L"[%llu] [ProcessTerminate] [%u] Path = ERROR",
            timestamp.QuadPart, HandleToUlong(ProcessId));
    }

    CommSendString(&message);
    ExFreePoolWithTag(message.Buffer, UTILS_TAG_UNICODE_STRING);

}

static VOID
ProcFltNotifyRoutine(
    _Inout_ PEPROCESS Process,
    _In_ HANDLE ProcessId,
    _Inout_opt_ PPS_CREATE_NOTIFY_INFO CreateInfo
)
{
    UNREFERENCED_PARAMETER(Process);

    if (!gDrv.MonitoringStarted)
    {
        // Monitoring is not started. We will simply exit the routine
        return;
    }

    if (CreateInfo)
    {
        ProcFltSendMessageProcessCreate(ProcessId, CreateInfo);
    }
    else
    {
        ProcFltSendMessageProcessTerminate(ProcessId);
    }
}

NTSTATUS
ProcessFilterInitialize()
{
    return PsSetCreateProcessNotifyRoutineEx(ProcFltNotifyRoutine, FALSE);
}

NTSTATUS
ProcessFilterUninitialize()
{
    return PsSetCreateProcessNotifyRoutineEx(ProcFltNotifyRoutine, TRUE);
}