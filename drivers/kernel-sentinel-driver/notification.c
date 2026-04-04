//
//   Copyright (C) 2019-2026 BitDefender S.R.L.
//   Author(s)    : Andrei-Marius MUNTEA(amuntea@bitdefender.com)
//                : Radu PORTASE(rportase@bitdefender.com)
//                : Vlad-Alexandru BARTOLOMEI (vlad.bartolomei@outlook.com)
//
#include "include\notification.h"

//
// Global Data
//
PFUNC_ZwQueryInformationProcess pfnZwQueryInformationProcess = NULL;
OB_CALLBACK_REGISTRATION    gCallbackRegistration;
PVOID                       gRegistrationHandle = NULL;
UNICODE_STRING              gRegAltitude = RTL_CONSTANT_STRING(L"389022.1"); // over bdprivmon.sys | Bitdefender SRL :)
PLARGE_INTEGER              gRegistryCookie = { 0 };

NTSTATUS
GetImagePathFromOpenHandle(
    _In_  HANDLE hProcess,
    _Out_ PUNICODE_STRING* ProcessPath
)
{
    NTSTATUS status = STATUS_UNSUCCESSFUL;
    ULONG dwObjectNameSize = 0;
    PUNICODE_STRING pProcessPath = NULL;

    __try
    {
        // get the size of the process name
        status = pfnZwQueryInformationProcess(hProcess,
            ProcessImageFileName, NULL,
            dwObjectNameSize, &dwObjectNameSize);
        if (STATUS_INFO_LENGTH_MISMATCH != status)
        {
            __leave;
        }

        // allocate required space
        pProcessPath = (PUNICODE_STRING)ExAllocatePool2(NonPagedPool, dwObjectNameSize, TAG_UCH);
        
        if (!pProcessPath)
        {
            status = STATUS_INSUFFICIENT_RESOURCES;
            __leave;
        }

        // get the name
        status = pfnZwQueryInformationProcess(hProcess, ProcessImageFileName,
            pProcessPath, dwObjectNameSize, &dwObjectNameSize);
        if (!NT_SUCCESS(status))
        {
            __leave;
        }
        *ProcessPath = pProcessPath;
        status = STATUS_SUCCESS;
    }
    __finally
    {
        if (!NT_SUCCESS(status))
        {
            if (pProcessPath)
            {
                ExFreePoolWithTag(pProcessPath, TAG_UCH);
            }
        }
    }
    return status;
}

NTSTATUS
GetCurrentProcessImagePath(
    _Out_ PUNICODE_STRING* ProcessPath
)
{
    NTSTATUS status = STATUS_UNSUCCESSFUL;
    HANDLE hProcess = NULL;

    __try
    {
        PEPROCESS currentProcess = PsGetCurrentProcess();
        if (!currentProcess)
        {
            __leave;
        }

        status = ObOpenObjectByPointer(currentProcess, OBJ_KERNEL_HANDLE,
            NULL, PROCESS_ALL_ACCESS, *PsProcessType, KernelMode,
            &hProcess);
        if (!NT_SUCCESS(status))
        {
            DrvLogError("ObOpenObjectByPointer failed with status 0x%X\n", status);
            __leave;
        }

        status = GetImagePathFromOpenHandle(hProcess, ProcessPath);
        if (!NT_SUCCESS(status))
        {
            DrvLogError("GetImagePathFromOpenHandle failed with status 0x%X\n", status);
            __leave;
        }

    }
    __finally
    {
        if (hProcess)
        {
            ZwClose(hProcess);
        }
    }

    return status;
}

VOID PrintOperationHeader(
    _In_ PSTR OperationName
)
{
    PUNICODE_STRING pProcessPath = NULL;
    NTSTATUS status = STATUS_UNSUCCESSFUL;
    __try
    {
        status = GetCurrentProcessImagePath(&pProcessPath);
        if (!NT_SUCCESS(status))
        {
            DrvLogError("GetCurrentProcessImagePath failed with status 0x%X\n", status);
            __leave;
        }

        DrvLogInfo("%s: %wZ\n", OperationName, pProcessPath);
    }
    __finally
    {
        if (pProcessPath)
        {
            ExFreePoolWithTag(pProcessPath, TAG_UCH);
        }
    }

}



// 
// Method to get old key path, pre-renaming
VOID
PrintKeyPath(
    _In_ PVOID KeyObject
)
{
    NTSTATUS ntStatus;
    ULONG_PTR objectId;
    PUNICODE_STRING keyPath = NULL;

    if (KeyObject == NULL) {
        DrvLogError("No Key Object provided to get it's key name!\r\n");
        return;
    }

    ntStatus = CmCallbackGetKeyObjectIDEx(
        gRegistryCookie,
        KeyObject,
        &objectId,
        &keyPath,
        0
    );

    if (NT_SUCCESS(ntStatus) && keyPath != NULL) {
        DrvLogInfo("   Old Name: %wZ\r\n", keyPath);
        CmCallbackReleaseKeyObjectIDEx(keyPath);
    }
    else {
        DrvLogError("CmCallbackGetKeyObjectIDEx failed: 0x%08X\r\n", ntStatus);
    }
}

//
// // Callback Registration
// Registry notification
//
NTSTATUS
CmRegistryCallback(
    _In_     PVOID CallbackContext,
    _In_opt_ PVOID Argument1,
    _In_opt_ PVOID Argument2
)
/*++
    Class                            ||  Structure:
    ===============================================================
    RegNtDeleteKey                   ||  REG_DELETE_KEY_INFORMATION
    RegNtPreDeleteKey                ||  REG_DELETE_KEY_INFORMATION
    RegNtPostDeleteKey               ||  REG_POST_OPERATION_INFORMATION
    RegNtSetValueKey                 ||  REG_SET_VALUE_KEY_INFORMATION
    RegNtPreSetValueKey              ||  REG_SET_VALUE_KEY_INFORMATION
    RegNtPostSetValueKey             ||  REG_POST_OPERATION_INFORMATION
    RegNtDeleteValueKey              ||  REG_DELETE_VALUE_KEY_INFORMATION
    RegNtPreDeleteValueKey           ||  REG_DELETE_VALUE_KEY_INFORMATION
    RegNtPostDeleteValueKey          ||  REG_POST_OPERATION_INFORMATION
    RegNtSetInformationKey           ||  REG_SET_INFORMATION_KEY_INFORMATION
    RegNtPreSetInformationKey        ||  REG_SET_INFORMATION_KEY_INFORMATION
    RegNtPostSetInformationKey       ||  REG_POST_OPERATION_INFORMATION
    RegNtRenameKey                   ||  REG_RENAME_KEY_INFORMATION
    RegNtPreRenameKey                ||  REG_RENAME_KEY_INFORMATION
    RegNtPostRenameKey               ||  REG_POST_OPERATION_INFORMATION
    RegNtEnumerateKey                ||  REG_ENUMERATE_KEY_INFORMATION
    RegNtPreEnumerateKey             ||  REG_ENUMERATE_KEY_INFORMATION
    RegNtPostEnumerateKey            ||  REG_POST_OPERATION_INFORMATION
    RegNtEnumerateValueKey           ||  REG_ENUMERATE_VALUE_KEY_INFORMATION
    RegNtPreEnumerateValueKey        ||  REG_ENUMERATE_VALUE_KEY_INFORMATION
    RegNtPostEnumerateValueKey       ||  REG_POST_OPERATION_INFORMATION
    RegNtQueryKey                    ||  REG_QUERY_KEY_INFORMATION
    RegNtPreQueryKey                 ||  REG_QUERY_KEY_INFORMATION
    RegNtPostQueryKey                ||  REG_POST_OPERATION_INFORMATION
    RegNtQueryValueKey               ||  REG_QUERY_VALUE_KEY_INFORMATION
    RegNtPreQueryValueKey            ||  REG_QUERY_VALUE_KEY_INFORMATION
    RegNtPostQueryValueKey           ||  REG_POST_OPERATION_INFORMATION
    RegNtQueryMultipleValueKey       ||  REG_QUERY_MULTIPLE_VALUE_KEY_INFORMATION
    RegNtPreQueryMultipleValueKey    ||  REG_QUERY_MULTIPLE_VALUE_KEY_INFORMATION
    RegNtPostQueryMultipleValueKey   ||  REG_POST_OPERATION_INFORMATION
    RegNtPreCreateKey                ||  REG_PRE_CREATE_KEY_INFORMATION
    RegNtPreCreateKeyEx              ||  REG_CREATE_KEY_INFORMATION**
    RegNtPostCreateKey               ||  REG_POST_CREATE_KEY_INFORMATION
    RegNtPostCreateKeyEx             ||  REG_POST_OPERATION_INFORMATION
    RegNtPreOpenKey                  ||  REG_PRE_OPEN_KEY_INFORMATION**
    RegNtPreOpenKeyEx                ||  REG_OPEN_KEY_INFORMATION
    RegNtPostOpenKey                 ||  REG_POST_OPEN_KEY_INFORMATION
    RegNtPostOpenKeyEx               ||  REG_POST_OPERATION_INFORMATION
    RegNtKeyHandleClose              ||  REG_KEY_HANDLE_CLOSE_INFORMATION
    RegNtPreKeyHandleClose           ||  REG_KEY_HANDLE_CLOSE_INFORMATION
    RegNtPostKeyHandleClose          ||  REG_POST_OPERATION_INFORMATION
    RegNtPreFlushKey                 ||  REG_FLUSH_KEY_INFORMATION
    RegNtPostFlushKey                ||  REG_POST_OPERATION_INFORMATION
    RegNtPreLoadKey                  ||  REG_LOAD_KEY_INFORMATION
    RegNtPostLoadKey                 ||  REG_POST_OPERATION_INFORMATION
    RegNtPreUnLoadKey                ||  REG_UNLOAD_KEY_INFORMATION
    RegNtPostUnLoadKey               ||  REG_POST_OPERATION_INFORMATION
    RegNtPreQueryKeySecurity         ||  REG_QUERY_KEY_SECURITY_INFORMATION
    RegNtPostQueryKeySecurity        ||  REG_POST_OPERATION_INFORMATION
    RegNtPreSetKeySecurity           ||  REG_SET_KEY_SECURITY_INFORMATION
    RegNtPostSetKeySecurity          ||  REG_POST_OPERATION_INFORMATION
    RegNtCallbackObjectContextCleanup||  REG_CALLBACK_CONTEXT_CLEANUP_INFORMATION
    RegNtPreRestoreKey               ||  REG_RESTORE_KEY_INFORMATION
    RegNtPostRestoreKey              ||  REG_RESTORE_KEY_INFORMATION
    RegNtPreSaveKey                  ||  REG_SAVE_KEY_INFORMATION
    RegNtPostSaveKey                 ||  REG_SAVE_KEY_INFORMATION
    RegNtPreReplaceKey               ||  REG_REPLACE_KEY_INFORMATION
    RegNtPostReplaceKey              ||  REG_REPLACE_KEY_INFORMATION
    RegNtPostCreateKeyEx             ||  REG_POST_OPERATION_INFORMATION
--*/
{
    UNREFERENCED_PARAMETER(CallbackContext); // not using a context yet

    REG_NOTIFY_CLASS regNotifyClass = (REG_NOTIFY_CLASS)(SIZE_T)Argument1;
    PVOID pParameters = Argument2;
    PVOID object = NULL;

    if (Argument2 == NULL) {
        DrvLogCritical("Argument 2 is NULL!");
        return STATUS_INVALID_PARAMETER;
    }

    switch (regNotifyClass) // this is the short version, i kept just what i need for the assignment
    {
    case RegNtPreRenameKey:
    {
        PREG_RENAME_KEY_INFORMATION info = ((PREG_RENAME_KEY_INFORMATION)pParameters);

        DrvLogInfo(">>> RegNtPreRenameKey\r\n");
        PrintKeyPath(info->Object);
        DrvLogInfo("    New Name: %wZ\r\n", info->NewName);
        break;
    }
    case RegNtPostRenameKey:
    {
        PREG_POST_OPERATION_INFORMATION info = ((PREG_POST_OPERATION_INFORMATION)pParameters);
        NTSTATUS opStatus = info->Status;
        DrvLogInfo("<<< RegNtPostRenameKey\r\n");
        DrvLogInfo("    Operation Status: 0x%08X (%s)\r\n", opStatus, NT_SUCCESS(opStatus) ? "SUCCESS" : "FAILED");
        break;
    }
    case RegNtPostQueryValueKey:
        object = ((PREG_POST_OPERATION_INFORMATION)pParameters)->Object;
        break;
    case RegNtCallbackObjectContextCleanup:
        object = ((PREG_CALLBACK_CONTEXT_CLEANUP_INFORMATION)pParameters)->Object;
        break;
    default:
        break;
    }

    if (regNotifyClass == RegNtQueryValueKey ||
        regNotifyClass == RegNtPreQueryValueKey ||
        regNotifyClass == RegNtPostQueryValueKey)
    {
        // registry query is too spammy to display in debugger
        return STATUS_SUCCESS;
    }

    PrintOperationHeader("ObPreOperationCallback");
    DrvLogInfo("REG_NOTIFY_CLASS: %d\n", regNotifyClass);
    if (object)
    {
        ULONG_PTR objectId;
        PUNICODE_STRING objectName;
        NTSTATUS status = STATUS_UNSUCCESSFUL;
        status = CmCallbackGetKeyObjectIDEx(gRegistryCookie, object, &objectId, &objectName, 0);
        if (!NT_SUCCESS(status))
        {
            DrvLogError("CmCallbackGetKeyObjectIDEx failed with status = 0x%X\n", status);
        }
        else
        {
            DrvLogInfo("Key: %wZ\n", objectName);
            CmCallbackReleaseKeyObjectIDEx(objectName);
        }
    }

    return STATUS_SUCCESS;
}

//
// Object notification
//
OB_PREOP_CALLBACK_STATUS
ObPreOperationCallback(
    _In_    PVOID RegistrationContext,
    _Inout_ POB_PRE_OPERATION_INFORMATION PreInfo
)
{
    UNREFERENCED_PARAMETER(RegistrationContext);
    UNREFERENCED_PARAMETER(PreInfo);

    if (!PreInfo ||
        !PreInfo->Object ||
        PreInfo->ObjectType != *PsProcessType)
    {
        return OB_PREOP_SUCCESS;
    }

    // Skip requests from the process itself
    if (PreInfo->Object == PsGetCurrentProcess())
    {
        return OB_PREOP_SUCCESS;
    }

    // Skip requests from the kernel. Avoids recursion and stack exhaustion
    if (PreInfo->KernelHandle == 1)
    {
        return OB_PREOP_SUCCESS;
    }

    PrintOperationHeader("ObPreOperationCallback");
    return OB_PREOP_SUCCESS;
}

VOID
ObPostOperationCallback(
    _In_ PVOID RegistrationContext,
    _In_ POB_POST_OPERATION_INFORMATION OperationInformation
)
{
    UNREFERENCED_PARAMETER(RegistrationContext);
    UNREFERENCED_PARAMETER(OperationInformation);
    if (OperationInformation->KernelHandle == 1)
    {
        // avoid recursion
        return;
    }

    PrintOperationHeader("ObPostOperationCallback");
    return;
}
