#include <fltKernel.h>
#include <wdm.h>

#include "include\my_driver.h"
#include "include\communication.h"
#include "include\communication_protocol.h"
#include "include\filter\process.h"
#include "include\filter\thread.h"
#include "include\filter\image.h"
#include "include\filter\registry.h"
#include "include\filter\file.h"
#include "trace.h"
#include "wikdd6k.tmh"

GLOBAL_DATA gDrv;

//PFLT_FILTER gFilterRegistration = NULL;
//PDRIVER_OBJECT gDriverObject = NULL;
//PFLT_PORT gServerPort = NULL;
//PFLT_PORT gClientPort = NULL;
//DRIVER_INITIALIZE DriverEntry;

/*VOID
D*/

//Precreate(
//    FltObject,
//    PVOID* CompletionContext // ca si call context
//) {
//    PFLT_FILE_NAME_INFORMATION fi = NULL;
//
//    FltGetFileNameInformation(
//
//    ); // rutina specifica pt operatii cu fisiere si pt callbacks
//
//    return some_status;
//
//    FLT_PREOP_SUCCESS_WITH_CALLBACK; // avem o treaba, se face callback
//    FLT_PREOP_SUCCESS_NO_CALLBACK; // eu mi-am terminat treaba
//
//}
//
//CONST FLT_OPERATION_REGISTRATION callbacks[] = {
//    // paging io, dam cu 0
//    PreCreate, 
//    PostCreate
//}
NTSTATUS
MyFilterUnload(
    _In_ FLT_FILTER_UNLOAD_FLAGS Flags
)
{
    //WikddLogInfo("Unloading driver. Flags = 0x%x", Flags);

    /*if (gClientPort)
    {
        FltCloseClientPort();
        gClientPort = NULL;
    }

    if (gServerPort) {
        FltCloseServerPort(..);
        gServerPort
    }*/
    FltUnregisterFilter(gDrv.FilterHandle);
    TpUninitialize(&gDrv.ThreadPool);
    WPP_CLEANUP(gDrv.DriverObject);

    return STATUS_SUCCESS;
}


CONST FLT_OPERATION_REGISTRATION callbacks[] = {
    { IRP_MJ_CREATE,
      0,
      MyFilterPreOperation,
      MyFilterPostOperation },

    { IRP_MJ_CLOSE,
      0,
      MyFilterPreOperation,
      MyFilterPostOperation },

    { IRP_MJ_CLEANUP,
      0,
      MyFilterPreOperation,
      MyFilterPostOperation },

    { IRP_MJ_READ,
      0,
      MyFilterPreOperationSynchronize,
      MyFilterPostOperation },

    { IRP_MJ_WRITE,
      0,
      MyFilterPreOperationSynchronize,
      MyFilterPostOperation },

    { IRP_MJ_SET_INFORMATION,
      0,
      MyFilterPreOperationSynchronize,
      MyFilterPostOperation },

    { IRP_MJ_OPERATION_END }
};

CONST FLT_REGISTRATION FilterRegistration = {

    sizeof(FLT_REGISTRATION),           //  Size
    FLT_REGISTRATION_VERSION,           //  Version
    0,                                  //  Flags

    NULL,                               //  Context
    callbacks,                          //  Operation callbacks

    MyFilterUnload,                     //  MiniFilterUnload

    NULL,                               //  InstanceSetup
    NULL,                               //  InstanceQueryTeardown
    NULL,                               //  InstanceTeardownStart
    NULL,                               //  InstanceTeardownComplete

    NULL,                               //  GenerateFileName
    NULL,                               //  GenerateDestinationFileName
    NULL                                //  NormalizeNameComponent
};

NTSTATUS
DriverEntry(
    _In_ PDRIVER_OBJECT DriverObject,
    _In_ PUNICODE_STRING RegistryPath
)
{
    WPP_INIT_TRACING(DriverObject, RegistryPath);

    WikddLogInfo("Starting driver ...");

    // 
    // Initialize global data. 
    //
    gDrv.DriverObject = DriverObject;
    UNICODE_STRING altitude = RTL_CONSTANT_STRING(L"370030.1");
    gDrv.Altitude = altitude;
    gDrv.MonitoringFlags = commNone;
    TpInit(&gDrv.ThreadPool, MAX_NUMBER_THREADS);

    //
    // We will need ZwQueryInformationProcess for process names
    //

    UNICODE_STRING ustrFunctionName = RTL_CONSTANT_STRING(L"ZwQueryInformationProcess");
    gDrv.pfnZwQueryInformationProcess = (PFUNC_ZwQueryInformationProcess)(SIZE_T)MmGetSystemRoutineAddress(&ustrFunctionName);

    if (!gDrv.pfnZwQueryInformationProcess)
    {
        WikddLogError("Unable to resolve ZwQueryInformationProcess!");
        return STATUS_INSUFF_SERVER_RESOURCES;
    }

    //
    //  Register with FltMgr to tell it our callback routines
    //
    NTSTATUS status = FltRegisterFilter(
        DriverObject,
        &FilterRegistration,
        &gDrv.FilterHandle
    );
    if (!NT_SUCCESS(status))
    {
        WikddLogApiFailedNt(status, "FltRegisterFilter");
        return status;
    }

    FLT_ASSERT(NT_SUCCESS(status));
    if (NT_SUCCESS(status))
    {
        //
        //  Prepare communication layer
        //
        NTSTATUS status_init_comm_port = CommInitializeFilterCommunicationPort();
        if (NT_NOT_SUCCESS(status_init_comm_port)) {

            FltUnregisterFilter(gDrv.FilterHandle);
            return status_init_comm_port;
        }

        NTSTATUS status_init_proc_flt = ProcessFilterInitialize();
        if (NT_NOT_SUCCESS(status_init_proc_flt))
        {
            CommUninitializeFilterCommunicationPort();
            FltUnregisterFilter(gDrv.FilterHandle);
            return status_init_proc_flt;
        }

        NTSTATUS status_init_thread_flt = ThreadFilterInitialize();
        if (NT_NOT_SUCCESS(status_init_thread_flt))
        {
            ProcessFilterUninitialize();
            CommUninitializeFilterCommunicationPort();
            FltUnregisterFilter(gDrv.FilterHandle);
            return status_init_thread_flt;
        }

        NTSTATUS status_init_img_flt = ImageFilterInitialize();
        if (NT_NOT_SUCCESS(status_init_img_flt))
        {
            ThreadFilterUninitialize();
            ProcessFilterUninitialize();
            CommUninitializeFilterCommunicationPort();
            FltUnregisterFilter(gDrv.FilterHandle);
            return status_init_img_flt;
        }

        NTSTATUS status_init_reg_flt = RegistryFilterInitialize();
        if (NT_NOT_SUCCESS(status_init_reg_flt))
        {
            ImageFilterUninitialize();
            ThreadFilterUninitialize();
            ProcessFilterUninitialize();
            CommUninitializeFilterCommunicationPort();
            FltUnregisterFilter(gDrv.FilterHandle);
            return status_init_reg_flt;
        }


        //
        //  Start filtering i/o
        //
        status = FltStartFiltering(gDrv.FilterHandle);
        if (NT_NOT_SUCCESS(status))
        {
            CommUninitializeFilterCommunicationPort();
            ProcessFilterUninitialize();
            FltUnregisterFilter(gDrv.FilterHandle);
        }
    }

    return status;
}