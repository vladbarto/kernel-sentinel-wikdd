#include "..\include\filter\utils.h"
#include "..\include\filter\registry.h"
#include "registry.tmh"
//
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

    if (!gDrv.MonitoringStarted)
    {
        return STATUS_SUCCESS;
    }

    switch (regNotifyClass)
    {
    case RegNtPreSetValueKey:
        object = ((PREG_SET_VALUE_KEY_INFORMATION)pParameters)->Object;
        break;
    case RegNtPostSetValueKey:
        object = ((PREG_POST_OPERATION_INFORMATION)pParameters)->Object;
        break;
    case RegNtPreDeleteValueKey:
        object = ((PREG_DELETE_VALUE_KEY_INFORMATION)pParameters)->Object;
        break;
    case RegNtPostDeleteValueKey:
        object = ((PREG_POST_OPERATION_INFORMATION)pParameters)->Object;
        break;
    case RegNtPreDeleteKey:
        object = ((PREG_DELETE_KEY_INFORMATION)pParameters)->Object;
        break;
    case RegNtPostDeleteKey:
        object = ((PREG_POST_OPERATION_INFORMATION)pParameters)->Object;
        break;
    case RegNtPostLoadKey:
        object = ((PREG_POST_OPERATION_INFORMATION)pParameters)->Object;
        break;
    case RegNtPostUnLoadKey:
        object = ((PREG_POST_OPERATION_INFORMATION)pParameters)->Object;
        break;
    case RegNtPreRenameKey:
        object = ((PREG_RENAME_KEY_INFORMATION)pParameters)->Object;
        break;
    case RegNtPostRenameKey:
        object = ((PREG_POST_OPERATION_INFORMATION)pParameters)->Object;
        break;
    case RegNtPostQueryValueKey:
        object = ((PREG_POST_OPERATION_INFORMATION)pParameters)->Object;
        break;
    case RegNtPostCreateKeyEx:
        object = ((PREG_POST_OPERATION_INFORMATION)pParameters)->Object;
        break;
    case RegNtPostOpenKeyEx:
        object = ((PREG_POST_OPERATION_INFORMATION)pParameters)->Object;
        break;
    case RegNtPreSaveKey:
        object = ((PREG_SAVE_KEY_INFORMATION)pParameters)->Object;
        break;

    case RegNtPostSaveKey:
        object = ((PREG_POST_OPERATION_INFORMATION)pParameters)->Object;

        break;
    case RegNtPreQueryValueKey:
        object = ((PREG_QUERY_VALUE_KEY_INFORMATION)pParameters)->Object;
        break;
    case RegNtPreCreateKey:
        // object = ((PREG_PRE_CREATE_KEY_INFORMATION)pParameters)->Object;
        // object is not created yet
        break;
    case RegNtPreCreateKeyEx:
        // object = ((PREG_CREATE_KEY_INFORMATION)pParameters)->Object;
        // object is not created yet
        break;
    case RegNtPostCreateKey:
        object = ((PREG_POST_CREATE_KEY_INFORMATION)pParameters)->Object;
        break;
    case RegNtPreSetInformationKey:
        object = ((PREG_SET_INFORMATION_KEY_INFORMATION)pParameters)->Object;
        break;
    case RegNtPostSetInformationKey:
        object = ((PREG_POST_OPERATION_INFORMATION)pParameters)->Object;
        break;
    case RegNtPreEnumerateKey:
        object = ((PREG_ENUMERATE_KEY_INFORMATION)pParameters)->Object;
        break;
    case RegNtPostEnumerateKey:
        object = ((PREG_POST_OPERATION_INFORMATION)pParameters)->Object;
        break;
    case RegNtPreEnumerateValueKey:
        object = ((PREG_ENUMERATE_VALUE_KEY_INFORMATION)pParameters)->Object;
        break;
    case RegNtPostEnumerateValueKey:
        object = ((PREG_POST_OPERATION_INFORMATION)pParameters)->Object;
        break;
    case RegNtPreQueryKey:
        object = ((PREG_QUERY_KEY_INFORMATION)pParameters)->Object;
        break;
    case RegNtPostQueryKey:
        object = ((PREG_POST_OPERATION_INFORMATION)pParameters)->Object;
        break;
    case RegNtPreQueryMultipleValueKey:
        object = ((PREG_QUERY_MULTIPLE_VALUE_KEY_INFORMATION)pParameters)->Object;
        break;
    case RegNtPostQueryMultipleValueKey:
        object = ((PREG_POST_OPERATION_INFORMATION)pParameters)->Object;
        break;
    case RegNtPreOpenKey:
        // object = ((PREG_PRE_OPEN_KEY_INFORMATION)pParameters)->Object;
        // object is not created yet
        break;
    case RegNtPreOpenKeyEx:
        // object = ((PREG_OPEN_KEY_INFORMATION)pParameters)->Object;
        // object is not created yet
        break;
    case RegNtPostOpenKey:
        object = ((PREG_POST_OPEN_KEY_INFORMATION)pParameters)->Object;
        break;
    case RegNtPreKeyHandleClose:
        object = ((PREG_KEY_HANDLE_CLOSE_INFORMATION)pParameters)->Object;
        break;
    case RegNtPostKeyHandleClose:
        object = ((PREG_POST_OPERATION_INFORMATION)pParameters)->Object;
        break;
    case RegNtPreFlushKey:
        object = ((PREG_FLUSH_KEY_INFORMATION)pParameters)->Object;
        break;
    case RegNtPostFlushKey:
        object = ((PREG_POST_OPERATION_INFORMATION)pParameters)->Object;
        break;
    case RegNtPreLoadKey:
        object = ((PREG_LOAD_KEY_INFORMATION)pParameters)->Object;
        break;
    case RegNtPreUnLoadKey:
        object = ((PREG_UNLOAD_KEY_INFORMATION)pParameters)->Object;
        break;
    case RegNtPreQueryKeySecurity:
        object = ((PREG_QUERY_KEY_SECURITY_INFORMATION)pParameters)->Object;
        break;
    case RegNtPostQueryKeySecurity:
        object = ((PREG_POST_OPERATION_INFORMATION)pParameters)->Object;
        break;
    case RegNtPreSetKeySecurity:
        object = ((PREG_SET_KEY_SECURITY_INFORMATION)pParameters)->Object;
        break;
    case RegNtPostSetKeySecurity:
        object = ((PREG_POST_OPERATION_INFORMATION)pParameters)->Object;
        break;
    case RegNtCallbackObjectContextCleanup:
        object = ((PREG_CALLBACK_CONTEXT_CLEANUP_INFORMATION)pParameters)->Object;
        break;
    case RegNtPreRestoreKey:
        object = ((PREG_RESTORE_KEY_INFORMATION)pParameters)->Object;
        break;
    case RegNtPostRestoreKey:
        object = ((PREG_RESTORE_KEY_INFORMATION)pParameters)->Object;
        break;
    case RegNtPreReplaceKey:
        object = ((PREG_REPLACE_KEY_INFORMATION)pParameters)->Object;
        break;
    case RegNtPostReplaceKey:
        object = ((PREG_REPLACE_KEY_INFORMATION)pParameters)->Object;
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

    if (object)
    {
        ULONG_PTR objectId;
        PUNICODE_STRING objectName;
        NTSTATUS status = STATUS_UNSUCCESSFUL;
        status = CmCallbackGetKeyObjectIDEx(&gDrv.RegistryCookie, object, &objectId, &objectName, 0);
        if (!NT_SUCCESS(status))
        {
            DrvLogError("CmCallbackGetKeyObjectIDEx failed with status = 0x%X\n", status);
        }
        else
        {
            DrvLogError("Key: %wZ\n", objectName);
            CmCallbackReleaseKeyObjectIDEx(objectName);
        }
    }

    return STATUS_SUCCESS;
}


NTSTATUS RegistryFilterInitialize()
{
    return CmRegisterCallbackEx(CmRegistryCallback,
        &gDrv.Altitude, gDrv.DriverObject, NULL, &gDrv.RegistryCookie, NULL);
}

NTSTATUS RegistryFilterUninitialize()
{
    return CmUnRegisterCallback(gDrv.RegistryCookie);
}