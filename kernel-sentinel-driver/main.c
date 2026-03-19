#include <ntddk.h>

VOID 
DriverUnload(
    _In_ PDRIVER_OBJECT DriverObject
)
{
    DriverObject;
    DbgPrintEx(
        DPFLTR_IHVDRIVER_ID,
        DPFLTR_ERROR_LEVEL,
        "Hello from driver load %d",
        150
    );
    __debugbreak();
}

NTSTATUS
DriverEntry(
    _In_ PDRIVER_OBJECT DriverObject,
    _In_ PUNICODE_STRING RegistryPath
)
{
    DriverObject;
    RegistryPath;

    DbgPrintEx(
        DPFLTR_IHVDRIVER_ID,
        DPFLTR_ERROR_LEVEL,
        "Hello from driver load %d",
        100
    );
    __debugbreak();

    DriverObject->DriverUnload = DriverUnload;

    DriverObject->MajorFunction[IRP_MJ_CREATE]         = MyCreateClose;
    DriverObject->MajorFunction[IRP_MJ_CLOSE]          = MyCreateClose;
    DriverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL] = MyCreateDeviceControl;

    return STATUS_SUCCESS;
}