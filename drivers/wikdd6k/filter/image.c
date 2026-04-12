#include <ntstrsafe.h>

#include "..\include\filter\utils.h"
#include "..\include\filter\image.h"

VOID
ImageLoadNotifyRoutine(
    _In_opt_ PUNICODE_STRING FullImageName,
    _In_     HANDLE ProcessId,   // pid into which image is being mapped
    _In_     PIMAGE_INFO ImageInfo
)
{
    PUNICODE_STRING pProcessPath = NULL;
    NTSTATUS status = STATUS_UNSUCCESSFUL;

    UNREFERENCED_PARAMETER(ProcessId);
    UNREFERENCED_PARAMETER(ImageInfo);

    if (!gDrv.MonitoringStarted || !ProcessId)
    {
        return;
    }

    __try
    {
        status = GetImagePathFromPid(ProcessId, &pProcessPath);
        if (!NT_SUCCESS(status))
        {
            LogError("GetCurrentProcessImagePath failed with status 0x%X\n", status);
            __leave;
        }

        WikddLogInfo("Image Notification for process %wZ. Path = %wZ", pProcessPath, FullImageName);
    }
    __finally
    {
        if (pProcessPath)
        {
            ExFreePoolWithTag(pProcessPath, UTILS_TAG_UNICODE_STRING);
        }
    }
}

NTSTATUS ImageFilterInitialize()
{
    return PsSetLoadImageNotifyRoutine(ImageLoadNotifyRoutine);
}

NTSTATUS ImageFilterUninitialize()
{
    return PsRemoveLoadImageNotifyRoutine(ImageLoadNotifyRoutine);
}