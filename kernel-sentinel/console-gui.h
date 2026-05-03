#pragma once

#include "common_includer.h"
#include "threadpool.h"
#include "process-listing.h"
#include "driver-caller.h"

// ********** Scroll down for Kernel Sentinel GUI ********** 

/////////////////////////////////////////
// Here goes GUI definitions for       //
// the first 5 labs implementing       //
// legacy NT drivers and experimenting //
/////////////////////////////////////////
int RunLegacyDriverFeatures();

///////////////////////////////////
// Here goes GUI definitions for //
// our Procmon-like monitoring   //
// tool -- Kernel Sentinel       //
///////////////////////////////////
#define MAX_ARGUMENTS 3
NTSTATUS RunKernelSentinelFeatures();