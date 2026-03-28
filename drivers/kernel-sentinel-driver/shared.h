#pragma once
#include <ntddk.h>

typedef struct _DRIVER_INPUT_UM {
	char InputBuffer[100];
	UINT32 NumberOfThreads;
} DRIVER_INPUT_UM, * PDRIVER_INPUT_UM;