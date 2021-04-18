#include "NTAssassin.h"

NTSTATUS NTAPI IO_Write(HANDLE FileHandle, ULONGLONG ByteOffset, PVOID Buffer, ULONG Length) {
    IO_STATUS_BLOCK stIOStatusBlock;
    LARGE_INTEGER   stLIOffset;
    stLIOffset.QuadPart = ByteOffset;
    return NtWriteFile(FileHandle, NULL, NULL, NULL, &stIOStatusBlock, Buffer, Length, &stLIOffset, NULL);
}

NTSTATUS NTAPI IO_WriteStringW(HANDLE FileHandle, PWSTR String) {
    return IO_Write(FileHandle, 0, String, Str_CcbLenExW(String, STRSAFE_MAX_CCH * sizeof(WCHAR)));
}

NTSTATUS NTAPI IO_WriteStringA(HANDLE FileHandle, PSTR String) {
    return IO_Write(FileHandle, 0, String, Str_CcbLenExA(String, STRSAFE_MAX_CCH));
}

NTSTATUS NTAPI IO_WriteLineW(HANDLE FileHandle, PWSTR String) {
    DWORD dwEOL = EOLW;
    NTSTATUS Status = IO_WriteStringW(FileHandle, String);
    return NT_SUCCESS(Status) ? IO_Write(FileHandle, 0, &dwEOL, sizeof(dwEOL)) : Status;
}

NTSTATUS NTAPI IO_WriteLineA(HANDLE FileHandle, PSTR String) {
    WORD dwEOL = EOLA;
    NTSTATUS Status = IO_WriteStringA(FileHandle, String);
    return NT_SUCCESS(Status) ? IO_Write(FileHandle, 0, &dwEOL, sizeof(dwEOL)) : Status;
}