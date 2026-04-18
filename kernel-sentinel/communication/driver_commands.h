#pragma once
//
//   Author(s)    : Radu PORTASE(rportase@bitdefender.com)
//
#include "communication_protocol.h"

//
// CmdGetDriverVersion
//
NTSTATUS
CmdGetDriverVersion(
    _Out_ PULONG DriverVersion
);

//
// CmdStartMonitoring
//

NTSTATUS
CmdStartMonitoring(
    _In_ MY_DRIVER_COMMAND_NOTIFICATION_TYPE_CODE NotificationType
);

//
// CmdStopMonitoring
//

NTSTATUS
CmdStopMonitoring(
    _In_ MY_DRIVER_COMMAND_NOTIFICATION_TYPE_CODE NotificationType
);
