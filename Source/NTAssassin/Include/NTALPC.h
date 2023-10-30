#pragma once

#include "NTAssassin_Base.h"

#include <rpc.h>

NTA_API BOOL NTAPI LPC_ServerRegisterIfSpec(_In_ RPC_IF_HANDLE IfSpec, _In_opt_ RPC_IF_CALLBACK_FN __RPC_FAR *IfCallback);

NTA_API VOID NTAPI LPC_ServerUnregisterIfSpec(_In_ RPC_IF_HANDLE IfSpec);

NTA_API BOOL NTAPI LPC_ServerListen(BOOL DontWait);

NTA_API DWORD NTAPI LPC_ServerGetClientPID(_In_ RPC_BINDING_HANDLE hBinding);
