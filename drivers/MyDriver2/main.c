#include <ntddk.h>

#define NT_DEVICE_NAME      L"\\Device\\SIOCTL"
#define DOS_DEVICE_NAME     L"\\GLOBAL??\\kernel-sentinel-driver"

#define NT_DEVICE_NAME_DRIVER_2 L"\\Device\\MYDRV2"

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
DriverControl(
    _In_ PDEVICE_OBJECT DeviceObject,
    _In_ PIRP Irp
) {
    DeviceObject;

    PIO_STACK_LOCATION  irpSp = { 0 };              // Pointer to current stack location
    NTSTATUS            ntStatus = STATUS_SUCCESS;  // Assume success
    ULONG               inBufLength = 0;            // Input buffer length
    ULONG               outBufLength = 0;           // Output buffer length

    DbgPrintEx(
        DPFLTR_IHVDRIVER_ID,
        DPFLTR_ERROR_LEVEL,
        "[Level 2] Hello from My Driver_2. NOOT NOOT!\r\n"
    );

    __debugbreak();

    irpSp = IoGetCurrentIrpStackLocation(Irp);
    inBufLength = irpSp->Parameters.DeviceIoControl.InputBufferLength;
    outBufLength = irpSp->Parameters.DeviceIoControl.OutputBufferLength;

    if (!inBufLength || !outBufLength) {
        ntStatus = STATUS_INVALID_PARAMETER;
        goto End;
    }

End:
    return ntStatus;
}

// -------------- Driver LOAD & UNLOAD ---------------------------------------------------------------------
VOID
DriverUnload(
    _In_ PDRIVER_OBJECT DriverObject
)
{
    DriverObject;
    DbgPrintEx(
        DPFLTR_IHVDRIVER_ID,
        DPFLTR_ERROR_LEVEL,
        "Hello from driver load %d\r\n",
        150
    );
    __debugbreak();
}


DriverEntry(
    _In_ PDRIVER_OBJECT DriverObject,
    _In_ PUNICODE_STRING RegistryPath
)
{
    DriverObject;
    RegistryPath;

    NTSTATUS ntStatus = STATUS_UNSUCCESSFUL;
    UNICODE_STRING ntDeviceName = { 0 };
    PDEVICE_OBJECT deviceObject = NULL;

    DbgPrintEx(
        DPFLTR_IHVDRIVER_ID,
        DPFLTR_ERROR_LEVEL,
        "Hello from driver load %d\r\n",
        100
    );
    __debugbreak();

    DriverObject->DriverUnload = DriverUnload;

    DriverObject->MajorFunction[IRP_MJ_CREATE] = MyCreateClose;
    DriverObject->MajorFunction[IRP_MJ_CLOSE] = MyCreateClose;
    DriverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL] = DriverControl;
    DriverObject->DriverUnload = DriverUnload;

    RtlInitUnicodeString(
        &ntDeviceName,
        NT_DEVICE_NAME_DRIVER_2
    );

    ntStatus = IoCreateDevice(
        DriverObject,               // Our Driver Object
        0,                          // We don't use a device extension
        &ntDeviceName,              // Device name "\Device\MYDRV2"
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

    return ntStatus;
}