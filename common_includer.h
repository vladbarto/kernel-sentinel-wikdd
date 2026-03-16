#pragma once

#include "trace.h"

#define _CRTDBG_MAP_ALLOC
#define WIN32_NO_STATUS
#include <Windows.h>
#include <winternl.h>
#include <intsafe.h>
#undef WIN32_NO_STATUS
#include <ntstatus.h>

#include <stdio.h>
#include <assert.h>
#include <crtdbg.h>
#include <stdlib.h>

#include "dynamic-load-library.h"