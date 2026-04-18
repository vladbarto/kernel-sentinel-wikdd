#pragma once
#include "..\..\include\my_driver.h"
#include "..\..\include\threadpool.h"

NTSTATUS
ImageFilterInitialize();

NTSTATUS
ImageFilterUninitialize();
