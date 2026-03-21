#pragma once
#include "common_includer.h"
#include "../kernel-sentinel-driver/ioctl_codes.h"

int __cdecl
UserModeDriverCaller(
	_In_ ULONG argc,
	_In_reads_(argc) PCHAR argv[]
);