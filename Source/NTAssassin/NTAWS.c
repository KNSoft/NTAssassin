#include "Include\NTAWS.h"

#pragma comment(lib, "ws2_32.lib")

BOOL NTAPI WS_Startup()
{
    WORD wVersionRequested = MAKEWORD(2, 2);
    WSADATA wsaData;
    INT iError = WSAStartup(wVersionRequested, &wsaData);
    if (iError == 0)
    {
        if (wsaData.wVersion != wVersionRequested)
        {
            WSACleanup();
            iError = WSAVERNOTSUPPORTED;
            goto Error;
        } else
        {
            return TRUE;
        }
    } else
    {
Error:
        WIE_SetLastError(iError);
        return FALSE;
    }
}

_Check_return_
SOCKET NTAPI WS_CreateIPv4ListenSocket(IPPROTO Protocol, PIN_ADDR Address, USHORT Port)
{
    INT iSockType;
    if (Protocol == IPPROTO_TCP)
    {
        iSockType = SOCK_STREAM;
    } else if (Protocol == IPPROTO_UDP)
    {
        iSockType = SOCK_DGRAM;
    } else
    {
        return INVALID_SOCKET;
    }

    SOCKET s;
    s = socket(AF_INET, iSockType, Protocol);
    if (s != INVALID_SOCKET)
    {
        SOCKADDR_IN service;
        service.sin_family = AF_INET;
        service.sin_addr = *Address;
        service.sin_port = WS_Htons(Port);
        if (bind(s, (PSOCKADDR)&service, sizeof(service)) != 0 ||
            listen(s, SOMAXCONN) != 0)
        {
            closesocket(s);
            s = INVALID_SOCKET;
        }
    }

    return s;
}
