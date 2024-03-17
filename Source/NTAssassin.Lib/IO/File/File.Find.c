﻿#include "NTAssassin.Lib.inl"

#define FILE_FIND_BUFFER_SIZE PAGE_SIZE

__inline
NTSTATUS NTAPI File_Find(
    _In_ HANDLE DirectoryHandle,
    _Out_ PVOID Buffer,
    _In_ ULONG BufferSize,
    _In_ FILE_INFORMATION_CLASS FileInformationClass,
    _In_opt_ PCUNICODE_STRING SearchFilter,
    _In_ BOOLEAN RestartScan,
    _Out_ PBOOL HasData)
{
    NTSTATUS Status;
    IO_STATUS_BLOCK IoStatusBlock;

    Status = NtQueryDirectoryFile(DirectoryHandle,
                                  NULL,
                                  NULL,
                                  NULL,
                                  &IoStatusBlock,
                                  Buffer,
                                  BufferSize,
                                  FileInformationClass,
                                  FALSE,
                                  RTL_CONST_CAST(PUNICODE_STRING)(SearchFilter),
                                  RestartScan);
    if (Status == STATUS_NO_MORE_FILES)
    {
        *HasData = FALSE;
        Status = STATUS_SUCCESS;
    } else if (Status == STATUS_BUFFER_OVERFLOW || NT_SUCCESS(Status))
    {
        *HasData = TRUE;
        Status = STATUS_SUCCESS;
    }

    return Status;
}

NTSTATUS NTAPI File_BeginFindByHandle(
    _Out_ PFILE_FIND FindData,
    _In_ HANDLE DirectoryHandle,
    _In_opt_ PCUNICODE_STRING SearchFilter,
    _In_ FILE_INFORMATION_CLASS FileInformationClass)
{
    NTSTATUS Status;
    PVOID Buffer;
    BOOL HasData;

    /* Allocate buffer */
    Buffer = RtlAllocateHeap(NtGetProcessHeap(), 0, FILE_FIND_BUFFER_SIZE);
    if (Buffer == NULL)
    {
        NtClose(DirectoryHandle);
        return STATUS_NO_MEMORY;
    }

    /* Find the first time */
    Status = File_Find(DirectoryHandle,
                       Buffer,
                       FILE_FIND_BUFFER_SIZE,
                       FileInformationClass,
                       SearchFilter,
                       TRUE,
                       &HasData);

    /* Assign input values if success, or cleanup and exit if fail */
    if (NT_SUCCESS(Status))
    {
        FindData->SearchFilter = SearchFilter;
        FindData->FileInformationClass = FileInformationClass;
        FindData->DirectoryHandle = DirectoryHandle;
        FindData->Buffer = Buffer;
        FindData->Length = FILE_FIND_BUFFER_SIZE;
        FindData->HasData = HasData;
    } else
    {
        RtlFreeHeap(NtGetProcessHeap(), 0, Buffer);
        NtClose(DirectoryHandle);
    }

    return Status;
}

NTSTATUS NTAPI File_BeginFind(
    _Out_ PFILE_FIND FindData,
    _In_ PCUNICODE_STRING DirectoryPath,
    _In_opt_ PCUNICODE_STRING SearchFilter,
    _In_ FILE_INFORMATION_CLASS FileInformationClass)
{
    NTSTATUS Status;
    HANDLE DirectoryHandle;

    Status = File_OpenDirectory(&DirectoryHandle,
                                DirectoryPath,
                                FILE_LIST_DIRECTORY | SYNCHRONIZE,
                                FILE_SHARE_READ | FILE_SHARE_WRITE);
    if (!NT_SUCCESS(Status))
    {
        return Status;
    }

    return File_BeginFindByHandle(FindData, DirectoryHandle, SearchFilter, FileInformationClass);
}

NTSTATUS NTAPI File_ContinueFind(_Inout_ PFILE_FIND FindData)
{
    return File_Find(FindData->DirectoryHandle,
                     FindData->Buffer,
                     FindData->Length,
                     FindData->FileInformationClass,
                     FindData->SearchFilter,
                     FALSE,
                     &FindData->HasData);
}

VOID NTAPI File_EndFind(_In_ PFILE_FIND FindData)
{
    RtlFreeHeap(NtGetProcessHeap(), 0, FindData->Buffer);
    NtClose(FindData->DirectoryHandle);
}