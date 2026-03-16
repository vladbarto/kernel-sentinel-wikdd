#pragma once

#include "common_includer.h"

typedef NTSTATUS (NTAPI* pNtQueryInformationProcess)(
	HANDLE ProcessHandle,
	PROCESSINFOCLASS ProcessInformationClass,
	PVOID ProcessInformation,
	ULONG ProcessInformationLength,
	PULONG ReturnLength
);

pNtQueryInformationProcess MyNtQueryInformationProcess = NULL;

BOOL LoadProcess(const char* ModuleName, const char* ProcessName);