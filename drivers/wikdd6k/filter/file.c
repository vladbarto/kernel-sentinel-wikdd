#include <ntstrsafe.h>
#include "..\include\filter\utils.h"
#include "..\include\filter\file.h"

//
// Functions to deal with instance management
//
NTSTATUS
OnInstanceSetup(
    _In_ PCFLT_RELATED_OBJECTS FltObjects,
    _In_ FLT_INSTANCE_SETUP_FLAGS Flags,
    _In_ DEVICE_TYPE VolumeDeviceType,
    _In_ FLT_FILESYSTEM_TYPE VolumeFilesystemType
)
{
    UNREFERENCED_PARAMETER(FltObjects);
    UNREFERENCED_PARAMETER(Flags);
    UNREFERENCED_PARAMETER(VolumeDeviceType);
    UNREFERENCED_PARAMETER(VolumeFilesystemType);

    PAGED_CODE();

    LogInfo("MyFilter!MyFilterInstanceSetup: Entered");

    return STATUS_SUCCESS;
}


NTSTATUS
OnQueryTeardown(
    _In_ PCFLT_RELATED_OBJECTS FltObjects,
    _In_ FLT_INSTANCE_QUERY_TEARDOWN_FLAGS Flags
)
{
    UNREFERENCED_PARAMETER(FltObjects);
    UNREFERENCED_PARAMETER(Flags);

    PAGED_CODE();

    LogInfo("MyFilter!MyFilterInstanceQueryTeardown: Entered");

    return STATUS_SUCCESS;
}


VOID
OnInstanceTeardownStart(
    _In_ PCFLT_RELATED_OBJECTS FltObjects,
    _In_ FLT_INSTANCE_TEARDOWN_FLAGS Flags
)
{
    UNREFERENCED_PARAMETER(FltObjects);
    UNREFERENCED_PARAMETER(Flags);

    PAGED_CODE();

    LogInfo("MyFilter!MyFilterInstanceTeardownStart: Entered");
}


VOID
OnInstanceTeardownComplete(
    _In_ PCFLT_RELATED_OBJECTS FltObjects,
    _In_ FLT_INSTANCE_TEARDOWN_FLAGS Flags
)
{
    UNREFERENCED_PARAMETER(FltObjects);
    UNREFERENCED_PARAMETER(Flags);

    PAGED_CODE();

    LogInfo("MyFilter!MyFilterInstanceTeardownComplete: Entered");
}

//
// Functions to monitor filesystem activity
//

FLT_PREOP_CALLBACK_STATUS
MyFilterPreOperation(
    _Inout_ PFLT_CALLBACK_DATA Data,
    _In_ PCFLT_RELATED_OBJECTS FltObjects,
    _Flt_CompletionContext_Outptr_ PVOID* CompletionContext
)
{
    UNREFERENCED_PARAMETER(Data);
    UNREFERENCED_PARAMETER(FltObjects);
    UNREFERENCED_PARAMETER(CompletionContext);

    if (gDrv.MonitoringStarted)
    {
        LogInfo("MyFilter!MyFilterPreOperation: Entered. Pid = 0x%X",
            HandleToUlong(FltGetRequestorProcessIdEx(Data)));
    }
    return FLT_PREOP_SUCCESS_WITH_CALLBACK;
}

FLT_PREOP_CALLBACK_STATUS
MyFilterPreOperationSynchronize(
    _Inout_ PFLT_CALLBACK_DATA Data,
    _In_ PCFLT_RELATED_OBJECTS FltObjects,
    _Flt_CompletionContext_Outptr_ PVOID* CompletionContext
)
{
    UNREFERENCED_PARAMETER(Data);
    UNREFERENCED_PARAMETER(FltObjects);
    UNREFERENCED_PARAMETER(CompletionContext);

    if (gDrv.MonitoringStarted)
    {
        LogInfo("MyFilter!MyFilterPreOperationSynchronize: Entered. Pid = 0x%X",
            HandleToUlong(FltGetRequestorProcessIdEx(Data)));
    }
    return FLT_PREOP_SYNCHRONIZE;
}


FLT_POSTOP_CALLBACK_STATUS
MyFilterPostOperation(
    _Inout_ PFLT_CALLBACK_DATA Data,
    _In_ PCFLT_RELATED_OBJECTS FltObjects,
    _In_opt_ PVOID CompletionContext,
    _In_ FLT_POST_OPERATION_FLAGS Flags
)
{
    UNREFERENCED_PARAMETER(Data);
    UNREFERENCED_PARAMETER(FltObjects);
    UNREFERENCED_PARAMETER(CompletionContext);
    UNREFERENCED_PARAMETER(Flags);

    if (gDrv.MonitoringStarted)
    {
        LogInfo("MyFilter!MyFilterPostOperation: Entered. Pid = 0x%X",
            HandleToUlong(FltGetRequestorProcessIdEx(Data)));
    }

    return FLT_POSTOP_FINISHED_PROCESSING;
}


FLT_PREOP_CALLBACK_STATUS
MyFilterPreOperationNoPostOperation(
    _Inout_ PFLT_CALLBACK_DATA Data,
    _In_ PCFLT_RELATED_OBJECTS FltObjects,
    _Flt_CompletionContext_Outptr_ PVOID* CompletionContext
)
{
    UNREFERENCED_PARAMETER(Data);
    UNREFERENCED_PARAMETER(FltObjects);
    UNREFERENCED_PARAMETER(CompletionContext);

    if (gDrv.MonitoringStarted)
    {
        LogInfo("MyFilter!MyFilterPreOperationNoPostOperation: Entered");
    }

    return FLT_PREOP_SUCCESS_NO_CALLBACK;
}