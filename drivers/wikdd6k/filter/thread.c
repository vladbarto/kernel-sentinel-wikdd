#include <ntstrsafe.h>
#include "..\include\filter\utils.h"
#include "..\include\filter\thread.h"

VOID
ThreadFilterNotifyRoutine(
    _In_ HANDLE ProcessId,
    _In_ HANDLE ThreadId,
    _In_ BOOLEAN Create
)
{
    UNREFERENCED_PARAMETER(ProcessId);
    UNREFERENCED_PARAMETER(ThreadId);
    UNREFERENCED_PARAMETER(Create);

    if (!gDrv.MonitoringStarted)
    {
        return;
    }

    PUNICODE_STRING pProcessPath = NULL;
    NTSTATUS status = STATUS_UNSUCCESSFUL;
    __try
    {
        status = GetImagePathFromPid(ProcessId, &pProcessPath);
        if (!NT_SUCCESS(status))
        {
            LogError("GetCurrentProcessImagePath failed with status 0x%X\n", status);
            __leave;
        }

        LogInfo("Thread Notification for process %wZ", pProcessPath);
    }
    __finally
    {
        if (pProcessPath)
        {
            ExFreePoolWithTag(pProcessPath, UTILS_TAG_UNICODE_STRING);
        }
    }
}

NTSTATUS ThreadFilterInitialize()
{
    return PsSetCreateThreadNotifyRoutine(ThreadFilterNotifyRoutine);

}

NTSTATUS ThreadFilterUninitialize()
{
    return PsRemoveCreateThreadNotifyRoutine(ThreadFilterNotifyRoutine);
}
