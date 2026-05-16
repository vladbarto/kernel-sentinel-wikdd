#include "..\include\filter\utils.h"
#include "..\include\filter\image.h"
#include "image.tmh"
VOID
ImageLoadNotifyRoutine(
    _In_opt_ PUNICODE_STRING FullImageName,
    _In_     HANDLE ProcessId,   // pid into which image is being mapped
    _In_     PIMAGE_INFO ImageInfo
)
{
    PUNICODE_STRING pProcessPath = NULL;
    NTSTATUS status = STATUS_UNSUCCESSFUL;

    UNREFERENCED_PARAMETER(ImageInfo);

    if (!gDrv.MonitoringStarted || !(gDrv.MonitoringFlags & commImageFilter))
    {
        return;
    }

    PMY_CONTEXT ctx = (PMY_CONTEXT)ExAllocatePoolWithTag(
        NonPagedPool,
        sizeof(MY_CONTEXT),
        UTILS_TAG_NOTIFICATION
    );

    if (!ctx) {
        DrvLogError(L"Failed to allocate notification work context");
        return;
    }
    RtlZeroMemory(ctx, sizeof(MY_CONTEXT));

    __try
    {
        status = GetImagePathFromPid(ProcessId, &pProcessPath);
        if (!NT_SUCCESS(status))
        {
            DrvLogError("GetCurrentProcessImagePath failed with status 0x%X\n", status);
            __leave;
        }

        ctx->NotificationType = commImageFilter;
        ctx->ProcessId = HandleToULong(ProcessId);
        ctx->OperationResult = status;

        // Copy process name
        if (pProcessPath && pProcessPath->Length > 0) 
        {
            ctx->ProcessName.Length = pProcessPath->Length;
            ctx->ProcessName.MaximumLength = pProcessPath->MaximumLength;
            ctx->ProcessName.Buffer = (PWCH)ExAllocatePoolWithTag(NonPagedPool, pProcessPath->MaximumLength, UTILS_TAG_NOTIFICATION);
            if (ctx->ProcessName.Buffer)
            {
                RtlCopyUnicodeString(&ctx->ProcessName, pProcessPath);
            }
        }

        // Copy image path
        if (FullImageName && FullImageName->Length > 0)
        {
            ctx->Data.MiniFilter.TargetPath.Length = FullImageName->Length;
            ctx->Data.MiniFilter.TargetPath.MaximumLength = FullImageName->MaximumLength;
            ctx->Data.MiniFilter.TargetPath.Buffer = (PWCH)ExAllocatePoolWithTag(NonPagedPool, FullImageName->MaximumLength, UTILS_TAG_NOTIFICATION);
            if(ctx->Data.MiniFilter.TargetPath.Buffer)
            {
                RtlCopyUnicodeString(&ctx->Data.MiniFilter.TargetPath, FullImageName);
            }
        }

        // Details
        RtlStringCbPrintfW(
            ctx->Data.MiniFilter.Details,
            sizeof(ctx->Data.MiniFilter.Details),
            L"ImageBase: 0x%p, ImageSize: 0x%IX",
            ImageInfo->ImageBase,
            ImageInfo->ImageSize
        );

        // Enqueue work item; let the KM ThreadPool deal with it
        status = TpEnqueueWorkItem(&gDrv.ThreadPool, MyWorkItemRoutine, ctx);
        if (NT_NOT_SUCCESS(status)) {
            DrvLogError(L"TpEnqueueWorkItem failed with 0x%X", status);
        }
        

        DrvLogError("Image Notification for process %wZ. Path = %wZ", pProcessPath, FullImageName);
    }
    __finally
    {
        if (NT_NOT_SUCCESS(status) && ctx) 
        {
            if (ctx->ProcessName.Buffer)
            {
                ExFreePoolWithTag(ctx->ProcessName.Buffer, UTILS_TAG_NOTIFICATION);
            }
            if (ctx->Data.MiniFilter.TargetPath.Buffer)
            {
                ExFreePoolWithTag(ctx->Data.MiniFilter.TargetPath.Buffer, UTILS_TAG_NOTIFICATION);
            }
        }

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