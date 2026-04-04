#pragma once
#include "shared.h"
#include "notification.tmh"
//
// Process name resolution
//
typedef
NTSTATUS
(NTAPI* PFUNC_ZwQueryInformationProcess) (
    _In_      HANDLE           ProcessHandle,
    _In_      PROCESSINFOCLASS ProcessInformationClass,
    _Out_     PVOID            ProcessInformation,
    _In_      ULONG            ProcessInformationLength,
    _Out_opt_ PULONG           ReturnLength
    );

extern PFUNC_ZwQueryInformationProcess pfnZwQueryInformationProcess;

//
// Global Data
//
extern OB_CALLBACK_REGISTRATION    gCallbackRegistration;
extern PVOID                       gRegistrationHandle;
extern UNICODE_STRING              gRegAltitude;
extern LARGE_INTEGER               gRegistryCookie;


NTSTATUS
GetImagePathFromOpenHandle(
    _In_  HANDLE hProcess,
    _Out_ PUNICODE_STRING* ProcessPath
);

NTSTATUS
GetCurrentProcessImagePath(
    _Out_ PUNICODE_STRING* ProcessPath
);

VOID PrintOperationHeader(
    _In_ PSTR OperationName
);

NTSTATUS
CmRegistryCallback(
    _In_     PVOID CallbackContext,
    _In_opt_ PVOID Argument1,
    _In_opt_ PVOID Argument2
);

OB_PREOP_CALLBACK_STATUS
ObPreOperationCallback(
    _In_    PVOID RegistrationContext,
    _Inout_ POB_PRE_OPERATION_INFORMATION PreInfo
);

VOID
ObPostOperationCallback(
    _In_ PVOID RegistrationContext,
    _In_ POB_POST_OPERATION_INFORMATION OperationInformation
);