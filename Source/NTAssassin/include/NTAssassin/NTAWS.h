#pragma once

#include "NTADef.h"

#include <WinSock2.h>

NTA_API BOOL NTAPI WS_Startup();

_Check_return_ NTA_API SOCKET NTAPI WS_CreateIPv4ListenSocket(IPPROTO Protocol, PIN_ADDR Address, USHORT Port);
