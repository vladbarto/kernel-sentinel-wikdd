#pragma once
#include <ntifs.h>

typedef struct _DRIVER_INPUT_UM {
	char InputBuffer[100];
	UINT32 NumberOfThreads;
} DRIVER_INPUT_UM, * PDRIVER_INPUT_UM;

/*
	Sev = 11 (binary)
	C = 1 (binary) (Custom)
	N = 0 (binary) (Mandatory from Microsoft)
	Facility = 0x001 (FACILITY_DEBUGGER)
	Code = 0x0001 (well, this is my first ntstatus error code)
	(Yes, I calculated that by hand)
*/
#define STATUS_THREADPOOL_ALREADY_UNINITIALIZED 0xE0010001;