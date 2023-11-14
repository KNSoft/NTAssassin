#include "Include\NTAIO.h"

BOOL NTAPI IO_Write(HANDLE FileHandle, ULONGLONG ByteOffset, _In_reads_bytes_(Length) PVOID Buffer, ULONG Length)
{
    IO_STATUS_BLOCK IoStatusBlock;
    LARGE_INTEGER Offset;
    NTSTATUS Status;

    Offset.QuadPart = ByteOffset;
    Status = NtWriteFile(FileHandle, NULL, NULL, NULL, &IoStatusBlock, Buffer, Length, &Offset, NULL);
    if (!NT_SUCCESS(Status))
    {
        WIE_SetLastStatus(Status);
        return FALSE;
    }

    return TRUE;
}
