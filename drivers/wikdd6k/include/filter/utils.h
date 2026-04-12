#pragma once
#include "..\..\include\my_driver.h"

#define UTILS_TAG_UNICODE_STRING 'hcu$'

NTSTATUS
GetImagePathFromOpenHandle(
    _In_  HANDLE hProcess,
    _Out_ PUNICODE_STRING* ProcessPath
);

NTSTATUS
GetImagePathFromPid(
    _In_  HANDLE Pid,
    _Out_ PUNICODE_STRING* ProcessPath
);

NTSTATUS
GetCurrentProcessImagePath(
    _Out_ PUNICODE_STRING* ProcessPath
);