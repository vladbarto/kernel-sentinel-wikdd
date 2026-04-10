#include <fltKernel.h>
#include <wdm.h>

#include "trace.h"
#include "wikdd6k.tmh"



PFLT_FILTER gFilterRegistration = NULL;
PDRIVER_OBJECT gDriverObject = NULL;
PFLT_PORT gServerPort = NULL;
PFLT_PORT gClientPort = NULL;
DRIVER_INITIALIZE DriverEntry;

/*VOID
D*/

Precreate(
    FltObject,
    PVOID* CompletionContext // ca si call context
) {
    PFLT_FILE_NAME_INFORMATION fi = NULL;

    FltGetFileNameInformation(

    ); // rutina specifica pt operatii cu fisiere si pt callbacks

    return some_status;

    FLT_PREOP_SUCCESS_WITH_CALLBACK; // avem o treaba, se face callback
    FLT_PREOP_SUCCESS_NO_CALLBACK; // eu mi-am terminat treaba

}

CONST FLT_OPERATION_REGISTRATION callbacks[] = {
    // paging io, dam cu 0
    PreCreate, 
    PostCreate
}
NTSTATUS
MyFilterUnload(
    _In_ FLT_FILTER_UNLOAD_FLAGS Flags
)
{
    WikddLogInfo("Unloading driver. Flags = 0x%x", Flags);

    if (gClientPort)
    {
        FltCloseClientPort();
        gClientPort = NULL;
    }

    if (gServerPort) {
        FltCloseServerPort(..);
        gServerPort
    }
    FltUnregisterFilter(gFilterRegistration);
    WPP_CLEANUP(gDriverObject);

    return STATUS_SUCCESS;
}


NTSTATUS
DriverEntry(
    _In_ PDRIVER_OBJECT DriverObject,
    _In_ PUNICODE_STRING RegistryPath
)
{
    gDriverObject = DriverObject;

    WPP_INIT_TRACING(DriverObject, RegistryPath);

    WikddLogInfo("Starting driver ...");

    CONST FLT_OPERATION_REGISTRATION callbacks[] = {
        { IRP_MJ_OPERATION_END }
    };

    CONST FLT_REGISTRATION fltReg = {

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
    
    NTSTATUS status = FltRegisterFilter(
        DriverObject,
        &fltReg,
        &gFilterRegistration
    );
    if (!NT_SUCCESS(status))
    {
        WikddLogApiFailedNt(status, "FltRegisterFilter");
        return status;
    }

    FltCreateCommunicationPort(
        gFilterRegistration,
        &gServerPort,

    )

    return STATUS_SUCCESS;
}