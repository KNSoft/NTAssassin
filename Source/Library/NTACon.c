﻿#include "Include\NTACon.h"

#include "Include\NTAStr.h"

DWORD NTAPI Con_Write(HANDLE ConHandle, _In_reads_bytes_(Length) PVOID Buffer, _In_ ULONG Length) {
    IO_STATUS_BLOCK IOStatus;
    NTSTATUS Status;
    Status = NtWriteFile(ConHandle, NULL, NULL, NULL, &IOStatus, Buffer, Length, NULL, NULL);
    if (NT_SUCCESS(Status)) {
        return (DWORD)IOStatus.Information;
    } else {
        WIE_SetLastStatus(Status);
        return 0;
    }
}

DWORD NTAPI Con_WriteString(HANDLE ConHandle, _In_z_ PCSTR String) {
    return Con_Write(ConHandle, (PVOID)String, (ULONG)Str_SizeA(String));
}

DWORD NTAPI Con_WriteLine(HANDLE ConHandle, _In_z_ PCSTR String) {
    WORD dwEOL = ANSI_EOL;
    DWORD dwRet = Con_WriteString(ConHandle, String);
    if (dwRet) {
        if (Con_Write(ConHandle, &dwEOL, sizeof(dwEOL))) {
            dwRet += sizeof(dwEOL);
        }
    }
    return dwRet;
}

DWORD NTAPI Con_VPrintf(HANDLE ConHandle, _In_z_ _Printf_format_string_ PCSTR Format, _In_ va_list ArgList) {
    CHAR szContent[1024];
    INT  i = Str_VPrintfA(szContent, Format, ArgList);
    return i > 0 ? Con_Write(ConHandle, szContent, i) : 0;
}

DWORD WINAPIV Con_Printf(HANDLE ConHandle, _In_z_ _Printf_format_string_ PCSTR Format, ...) {
    va_list args;
    va_start(args, Format);
    DWORD dwRet = Con_VPrintf(ConHandle, Format, args);
    va_end(args);
    return dwRet;
}

DWORD WINAPIV Con_PrintfLine(HANDLE ConHandle, _In_z_ _Printf_format_string_ PCSTR Format, ...) {
    WORD dwEOL = ANSI_EOL;
    va_list args;
    va_start(args, Format);
    DWORD dwRet = Con_VPrintf(ConHandle, Format, args);
    if (dwRet) {
        if (Con_Write(ConHandle, &dwEOL, sizeof(dwEOL))) {
            dwRet += sizeof(dwEOL);
        }
    }
    va_end(args);
    return dwRet;
}