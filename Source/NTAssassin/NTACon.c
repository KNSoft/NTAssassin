#include "include\NTAssassin\NTAssassin.h"

DWORD Con_Write(HANDLE ConHandle, PVOID Buffer, SIZE_T Length) {
    IO_STATUS_BLOCK stIOStatusBlock;
    return NT_SUCCESS(NtWriteFile(ConHandle, NULL, NULL, NULL, &stIOStatusBlock, Buffer, Length, NULL, NULL)) ? (DWORD)stIOStatusBlock.Information : 0;
}

DWORD Con_WriteString(HANDLE ConHandle, PCSTR String) {
    return Con_Write(ConHandle, (PVOID)String, Str_SizeA(String));
}

DWORD Con_WriteLine(HANDLE ConHandle, PCSTR String) {
    WORD dwEOL = EOLA;
    DWORD dwRet = Con_WriteString(ConHandle, String);
    if (dwRet) {
        if (Con_Write(ConHandle, &dwEOL, sizeof(dwEOL))) {
            dwRet += sizeof(dwEOL);
        }
    }
    return dwRet;
}

DWORD Con_VPrintf(HANDLE ConHandle, _In_z_ _Printf_format_string_ PCSTR Format, _In_ va_list ArgList) {
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
    WORD dwEOL = EOLA;
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