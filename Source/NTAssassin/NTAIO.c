#include "include\NTAssassin\NTAIO.h"
#include "include\NTAssassin\NTANT.h"
#include "include\NTAssassin\NTAStr.h"

BOOL NTAPI IO_Write(HANDLE FileHandle, ULONGLONG ByteOffset, _In_reads_bytes_(Length) PVOID Buffer, ULONG Length) {
    IO_STATUS_BLOCK stIOStatus;
    LARGE_INTEGER   stLIOffset;
    NTSTATUS        lStatus;
    stLIOffset.QuadPart = ByteOffset;
    lStatus = NtWriteFile(FileHandle, NULL, NULL, NULL, &stIOStatus, Buffer, Length, &stLIOffset, NULL);
    if (NT_SUCCESS(lStatus)) {
        return TRUE;
    } else {
        NT_SetLastStatus(lStatus);
        return FALSE;
    }
}

BOOL NTAPI IO_WriteStringW(HANDLE FileHandle, _In_z_ PCWSTR String) {
    return IO_Write(FileHandle, 0, (PVOID)String, (ULONG)Str_SizeW(String));
}

BOOL NTAPI IO_WriteStringA(HANDLE FileHandle, _In_z_ PCSTR String) {
    return IO_Write(FileHandle, 0, (PVOID)String, (ULONG)Str_SizeA(String));
}

BOOL NTAPI IO_WriteLineW(HANDLE FileHandle, _In_z_ PCWSTR String) {
    DWORD dwEOL = EOLW;
    BOOL bRet = IO_WriteStringW(FileHandle, String);
    return bRet ? IO_Write(FileHandle, 0, &dwEOL, sizeof(dwEOL)) : FALSE;
}

BOOL NTAPI IO_WriteLineA(HANDLE FileHandle, _In_z_ PCSTR String) {
    WORD dwEOL = EOLA;
    BOOL bRet = IO_WriteStringA(FileHandle, String);
    return bRet ? IO_Write(FileHandle, 0, &dwEOL, sizeof(dwEOL)) : FALSE;
}