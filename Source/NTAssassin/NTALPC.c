#include "Include\NTALPC.h"

#pragma comment(lib, "rpcrt4.lib")

BOOL NTAPI LPC_ServerRegisterIfSpec(_In_ RPC_IF_HANDLE IfSpec, _In_opt_ RPC_IF_CALLBACK_FN __RPC_FAR *IfCallback)
{
    RPC_STATUS Status;

    Status = RpcServerUseProtseqIfW((RPC_WSTR)L"ncalrpc", RPC_C_PROTSEQ_MAX_REQS_DEFAULT, IfSpec, NULL);
    if (Status != RPC_S_OK) {
        goto _Error;
    }

    Status = RpcServerRegisterIfEx(IfSpec, NULL, NULL, RPC_IF_ALLOW_LOCAL_ONLY, RPC_C_LISTEN_MAX_CALLS_DEFAULT, IfCallback);
    if (Status != RPC_S_OK) {
        LPC_ServerUnregisterIfSpec(IfSpec);
        goto _Error;
    }

    return TRUE;

_Error:
    WIE_SetLastError(Status);
    return FALSE;
}

VOID NTAPI LPC_ServerUnregisterIfSpec(_In_ RPC_IF_HANDLE IfSpec)
{
    #pragma warning(disable: 6031)
    RpcServerUnregisterIf(IfSpec, NULL, TRUE);
    #pragma warning(default: 6031)
}

BOOL NTAPI LPC_ServerListen(BOOL DontWait)
{
    RPC_STATUS Status = RpcServerListen(1, RPC_C_LISTEN_MAX_CALLS_DEFAULT, DontWait);
    if (Status == RPC_S_OK) {
        return TRUE;
    } else {
        WIE_SetLastError(Status);
        return FALSE;
    }
}

DWORD NTAPI LPC_ServerGetClientPID(_In_ RPC_BINDING_HANDLE hBinding)
{
    RPC_STATUS Status;
    RPC_CALL_ATTRIBUTES_V2_W clientAttr = { 2, RPC_QUERY_CLIENT_PID };
    Status = RpcServerInqCallAttributesW(hBinding, &clientAttr);
    if (Status == RPC_S_OK) {
        return (DWORD)(DWORD_PTR)clientAttr.ClientPID;
    } else {
        WIE_SetLastError(Status);
        return 0;
    }
}
