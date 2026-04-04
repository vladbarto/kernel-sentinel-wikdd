#include "ioctl_codes.h"
#include "include\threadpool.h"
#include "include\notification.h"
#include "main.tmh"

#define NT_DEVICE_NAME      L"\\Device\\SIOCTL"
#define DOS_DEVICE_NAME     L"\\GLOBAL??\\kernel-sentinel-driver"

#define NT_DEVICE_NAME_DRIVER_2 L"\\Device\\MYDRV2"




// -------------- Notification Routines ---------------------------------------------------------------------
VOID
MyNotifyRoutine(
    _Inout_     PEPROCESS Process,
    _In_        HANDLE ProcessId,
    _Inout_opt_ PPS_CREATE_NOTIFY_INFO CreateInfo
) {
    Process;
    ProcessId;
    CreateInfo;
    if (CreateInfo) {
        // process create
        DrvLogInfo("Process with id 0x%p created successfully! Other info:\r\n", ProcessId);
        DrvLogInfo("Image File Name = %wZ\r\nCommandLine = %wZ\r\n", CreateInfo->ImageFileName, CreateInfo->CommandLine);
        PrintOperationHeader("MyNotifyRoutine");
    }
    else {
        //process exit
        DrvLogInfo("Process with id 0x%p terminated", ProcessId);
    }
    
}


// -------------- Driver Create / Close, Create Device Control definitions ----------------------------------
NTSTATUS
MyCreateClose(
    _In_ PDEVICE_OBJECT DeviceObject,
    _In_ PIRP Irp
) {
    DeviceObject;

    __debugbreak();

    DbgPrintEx(
        DPFLTR_IHVDRIVER_ID,
        DPFLTR_ERROR_LEVEL,
        "Driver create or close triggered %d\r\n",
        180
    );

    Irp->IoStatus.Status = STATUS_SUCCESS;
    Irp->IoStatus.Information = 0;

    IoCompleteRequest(Irp, IO_NO_INCREMENT);

    return STATUS_SUCCESS;
}

NTSTATUS
MyCreateDeviceControl(                      // This is Driver_1 and at the same time a Dispatcher Driver  
    _In_ PDEVICE_OBJECT DeviceObject,
    _In_ PIRP Irp
) {
    DeviceObject;

    PIO_STACK_LOCATION  irpSp = { 0 };              // Pointer to current stack location
    NTSTATUS            ntStatus = STATUS_SUCCESS;  // Assume success
    ULONG               inBufLength = 0;            // Input buffer length
    ULONG               outBufLength = 0;           // Output buffer length

    __debugbreak();

    DbgPrintEx(
        DPFLTR_IHVDRIVER_ID,
        DPFLTR_ERROR_LEVEL,
        "Hello from MyCreateDeviceControl\r\n"
    );

    Irp->StackCount = 2; // 2 drivers, 2 stacks

    irpSp           = IoGetCurrentIrpStackLocation(Irp);
    inBufLength     = irpSp->Parameters.DeviceIoControl.InputBufferLength;
    outBufLength    = irpSp->Parameters.DeviceIoControl.OutputBufferLength;
    
    if (!inBufLength || !outBufLength) {
        ntStatus = STATUS_INVALID_PARAMETER;
        goto End;
    }

    PDRIVER_INPUT_UM input = (PDRIVER_INPUT_UM) Irp->AssociatedIrp.SystemBuffer;

    if (inBufLength < sizeof(DRIVER_INPUT_UM)) {
        Irp->IoStatus.Status = STATUS_BUFFER_TOO_SMALL;
        Irp->IoStatus.Information = 0;
        IoCompleteRequest(Irp, IO_NO_INCREMENT);
        return STATUS_BUFFER_TOO_SMALL;
    }

    MY_THREAD_POOL tp = { 0 };
    UINT32 numberOfThreads = input->NumberOfThreads;
    PMY_CONTEXT ctx = { 0 };

    switch (irpSp->Parameters.DeviceIoControl.IoControlCode) {
        case IOCTL_SIOCTL_METHOD_BUFFERED:
            DbgPrintEx(
                DPFLTR_IHVDRIVER_ID,
                DPFLTR_ERROR_LEVEL,
                "[Called IOCTL_SIOCTL_METHOD_BUFFERED]\r\nFirst IOCTL was called\r\n"
            );

            DbgPrintEx(
                DPFLTR_IHVDRIVER_ID,
                DPFLTR_ERROR_LEVEL,
                "Received: %s\r\n",
                (char*) Irp->AssociatedIrp.SystemBuffer
            );

            break;
        case IOCTL_SIOCTL_METHOD_BUFFERED_2:
            DbgPrintEx(
                DPFLTR_IHVDRIVER_ID,
                DPFLTR_ERROR_LEVEL,
                "[Called IOCTL_SIOCTL_METHOD_BUFFERED_2]\r\nSecond IOCTL was called\r\n"
            );

            DbgPrintEx(
                DPFLTR_IHVDRIVER_ID,
                DPFLTR_ERROR_LEVEL,
                "Received: %s\r\n",
                (char*)Irp->AssociatedIrp.SystemBuffer
            );

            break;
        case IOCTL_SIOCTL_METHOD_BUFFERED_DRIVER_2:
            DbgPrintEx(
                DPFLTR_IHVDRIVER_ID,
                DPFLTR_ERROR_LEVEL,
                "[Called IOCTL_SIOCTL_METHOD_BUFFERED_DRIVER_2]\r\nThird (driver 2) IOCTL was called\r\n"
            );

            DbgPrintEx(
                DPFLTR_IHVDRIVER_ID,
                DPFLTR_ERROR_LEVEL,
                "Received: %s\r\n",
                (char*)Irp->AssociatedIrp.SystemBuffer
            );

            __debugbreak();
            // Get address of another driver's device object
            PFILE_OBJECT FileObject2     = { 0 };
            PDEVICE_OBJECT DeviceObject2 = { 0 };
            UNICODE_STRING ntDeviceName2 = { 0 };
            RtlInitUnicodeString(
                &ntDeviceName2,
                NT_DEVICE_NAME_DRIVER_2
            );

            ntStatus = IoGetDeviceObjectPointer(
                &ntDeviceName2,
                FILE_ALL_ACCESS,
                &FileObject2,
                &DeviceObject2
            );

            if (ntStatus != STATUS_SUCCESS) {
                goto End;
            }

            // Before passing the request copy the data to the next driver
            IoCopyCurrentIrpStackLocationToNext(Irp);

            __debugbreak();
            // Pass request to driver 2
            IoSetNextIrpStackLocation(Irp); // irp stack pointer += 1, so to speak
            IoCallDriver(DeviceObject2, Irp);

            __debugbreak();
            ObDereferenceObject(DeviceObject2);

            break;
        case IOCTL_SIOCTL_METHOD_BUFFERED_TP_INIT:
            ntStatus = TpInit(&tp, numberOfThreads);
            if (!NT_SUCCESS(ntStatus)) {
                goto End;
            }
            break;
        case IOCTL_SIOCTL_METHOD_BUFFERED_TP_SUBMIT_WORK_ITEM:
            KeInitializeSpinLock(&ctx->SpinLock);
            ntStatus = TpEnqueueWorkItem(&tp, MyWorkItemRoutine, ctx);
            if (!NT_SUCCESS(ntStatus)) {
                goto End;
            }
            break;
        case IOCTL_SIOCTL_METHOD_BUFFERED_TP_UNLOAD:
            ntStatus = TpUninitialize(&tp);
            if (!NT_SUCCESS(ntStatus)) {
                goto End;
            }
            break;
        case IOCTL_SIOCTL_METHOD_BUFFERED_TP_TEST:
        {
            MY_THREAD_POOL tp_test = { 0 };
            MY_CONTEXT ctx_test = { 0 };
            NTSTATUS status_test = STATUS_UNSUCCESSFUL;

            status_test = TpInit(&tp_test, 5);
            if (!NT_SUCCESS(status_test))
            {
                goto End;
            }

            KeInitializeSpinLock(&ctx_test.SpinLock);
            ctx_test.Number = 0;

            for (int i = 0; i < 100000; ++i)
            {
                status_test = TpEnqueueWorkItem(&tp, MyWorkItemRoutine, &ctx);
                if (!NT_SUCCESS(status_test))
                {
                    goto End;
                }
            }

            status_test = TpUninitialize(&tp);

            /* If everything went well, this should output 100000000. */
            DbgPrintEx(
                DPFLTR_IHVDRIVER_ID,
                DPFLTR_ERROR_LEVEL,
                "Final number value = %d\r\n",
                ctx_test.Number
            );

            if (!NT_SUCCESS(status_test)) {
                goto End;
            }
            break;
        }
        default:
            break;
    }
    
    Irp->IoStatus.Status = STATUS_SUCCESS; // failing ntStatus should not be overriden at the end with success
End:
    Irp->IoStatus.Information = 0;

    //TpUninitialize(&tp);DOESN'T WORK; I risk leaks but At least works // disregard the status, close the TP either way (corner case: tp_init is called, but not tp_unload as well)
    IoCompleteRequest(Irp, IO_NO_INCREMENT);

    return Irp->IoStatus.Status;
}


// -------------- Driver LOAD & UNLOAD ---------------------------------------------------------------------
VOID 
DriverUnload(
    _In_ PDRIVER_OBJECT DriverObject
)
{
    DrvLogInfo("Driver unloading... \r\n");
    __debugbreak();
    NTSTATUS ntStatus = PsSetCreateProcessNotifyRoutineEx(MyNotifyRoutine, TRUE);
    if (!NT_SUCCESS(ntStatus)) {
        DrvLogError("PsSetCreateProcessNotifyRoutineEx failed in Driver Unload stage!");
        __debugbreak();
    }
    CmUnRegisterCallback(*gRegistryCookie);
    ObUnRegisterCallbacks(gRegistrationHandle);

    WPP_CLEANUP(DriverObject);
}

NTSTATUS
DriverEntry(
    _In_ PDRIVER_OBJECT DriverObject,
    _In_ PUNICODE_STRING RegistryPath
)
{
    WPP_INIT_TRACING (DriverObject, RegistryPath);

    NTSTATUS ntStatus = STATUS_UNSUCCESSFUL;
    UNICODE_STRING ntDeviceName = { 0 };
    UNICODE_STRING dosDeviceName = { 0 };
    PDEVICE_OBJECT deviceObject = NULL;

    DbgPrintEx(
        DPFLTR_IHVDRIVER_ID,
        DPFLTR_ERROR_LEVEL,
        "Hello from driver load %d\r\n",
        100
    );
    __debugbreak();

    DriverObject->DriverUnload = DriverUnload;

    DriverObject->MajorFunction[IRP_MJ_CREATE]          = MyCreateClose;
    DriverObject->MajorFunction[IRP_MJ_CLOSE]           = MyCreateClose;
    DriverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL]  = MyCreateDeviceControl;
    DriverObject->DriverUnload                          = DriverUnload;

    RtlInitUnicodeString(
        &ntDeviceName,
        NT_DEVICE_NAME
    );

    ntStatus = IoCreateDevice(
        DriverObject,               // Our Driver Object
        0,                          // We don't use a device extension
        &ntDeviceName,              // Device name "\Device\SIOCTL"
        FILE_DEVICE_UNKNOWN,        // Device type
        FILE_DEVICE_SECURE_OPEN,    // Device characteristics
        FALSE,                      // Not an exclusive device
        &deviceObject               // Returned ptr to Device Object
    );

    if (!NT_SUCCESS(ntStatus)) {
        DbgPrintEx(
            DPFLTR_IHVDRIVER_ID,
            DPFLTR_ERROR_LEVEL,
            "Couldn't create the device object\r\n"
        );
        return ntStatus;
    }

    RtlInitUnicodeString(
        &dosDeviceName,
        DOS_DEVICE_NAME
    );

    ntStatus = IoCreateSymbolicLink(
        &dosDeviceName,
        &ntDeviceName
    );

    if (!NT_SUCCESS(ntStatus)) {
        DbgPrintEx(
            DPFLTR_IHVDRIVER_ID,
            DPFLTR_ERROR_LEVEL,
            "Couldn't create the symlink\r\n"
        );
        IoDeleteDevice(deviceObject);
        return ntStatus;
    }

    //
    // Process Notification
    // 
    ntStatus = PsSetCreateProcessNotifyRoutineEx(MyNotifyRoutine, FALSE);
    if (!NT_SUCCESS(ntStatus)) {
        DrvLogWarning("PsSetCreateProcessNotifyRoutineEx failed with status 0x%x\r\n", ntStatus);
        WPP_CLEANUP(DriverObject);
        return ntStatus;
    }

    //
    // Registry notification
    //
    ntStatus = CmRegisterCallbackEx(
        CmRegistryCallback,
        &gRegAltitude, 
        DriverObject, 
        NULL, // ctx
        gRegistryCookie, 
        NULL
    );
    if (!NT_SUCCESS(ntStatus))
    {
        DrvLogError("PsSetLoadImageNotifyRoutine failed with status 0x%X\n", ntStatus);
        PsSetCreateProcessNotifyRoutineEx(MyNotifyRoutine, TRUE);
        return ntStatus;
    }

    //
    // Object notification
    //
    OB_OPERATION_REGISTRATION operationRegistrations[1] = { { 0 } };
    operationRegistrations[0].ObjectType = PsProcessType;
    operationRegistrations[0].Operations |= OB_OPERATION_HANDLE_CREATE;
    operationRegistrations[0].Operations |= OB_OPERATION_HANDLE_DUPLICATE;
    operationRegistrations[0].PreOperation = ObPreOperationCallback;
    operationRegistrations[0].PostOperation = ObPostOperationCallback;


    gCallbackRegistration.Version = OB_FLT_REGISTRATION_VERSION;
    gCallbackRegistration.OperationRegistrationCount = 1;
    gCallbackRegistration.Altitude = gRegAltitude;
    gCallbackRegistration.RegistrationContext = NULL;
    gCallbackRegistration.OperationRegistration = operationRegistrations;



    ntStatus = ObRegisterCallbacks(&gCallbackRegistration, &gRegistrationHandle);
    if (!NT_SUCCESS(ntStatus))
    {
        DrvLogError("ObRegisterCallbacks failed with status 0x%X", ntStatus);
        PsSetCreateProcessNotifyRoutineEx(MyNotifyRoutine, TRUE);
        CmUnRegisterCallback(*gRegistryCookie);
        return ntStatus;
    }

    return STATUS_SUCCESS;
}
