#pragma once
#include "common_includer.h"
#include "../drivers/kernel-sentinel-driver/ioctl_codes.h"
#include <string>

typedef struct _DRIVER_INPUT_UM {
	char InputBuffer[100];
	UINT32 NumberOfThreads;
} DRIVER_INPUT_UM, * PDRIVER_INPUT_UM;


#define MAX_NR_THREADS_ALLOWED 10

int __cdecl
UserModeDriverCaller(
	_In_ ULONG argc,
	_In_reads_(argc) PCHAR argv[]
);