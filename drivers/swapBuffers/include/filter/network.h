#pragma once
#include "..\..\include\my_driver.h"

/*
* First logical step: Register my network filter
* I don't start building a house by destroying it (unregister) :>
* Registers my filters: gAleAuth{Connect|ReceiveAccept}V{4|6}Filter
*/
NTSTATUS
DriverRegisterNetworkFilter();

/*
* Unregisters my filters: gAleAuth{Connect|ReceiveAccept}V{4|6}Filter
*/
VOID
DriverUnregisterNetworkFilter();