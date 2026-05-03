#pragma once

#include "trace.h"

#define _CRTDBG_MAP_ALLOC
#define WIN32_NO_STATUS
#include <Windows.h>
#include <intsafe.h>
#undef WIN32_NO_STATUS
#pragma warning(push)
#pragma warning(disable: 4005)
#include <ntstatus.h>
#pragma warning(pop)

#include <stdio.h>
#include <assert.h>
#include <crtdbg.h>
#include <stdlib.h>

#include <winioctl.h>
#include <string.h>
#include <strsafe.h>
#include <iostream>

#define NT_NOT_SUCCESS(status) !NT_SUCCESS(status)
//#include "dynamic-load-library.h"