#include "include\NTAssassin\NTAFile.h"
#include "include\NTAssassin\NTAEH.h"
#include "include\NTAssassin\NTAMem.h"

HANDLE NTAPI File_Create(_In_z_ PCWSTR FileName, HANDLE RootDirectory, ACCESS_MASK DesiredAccess, ULONG ShareAccess, ULONG CreateDisposition, ULONG CreateOptions) {
    UNICODE_STRING      stString;
    OBJECT_ATTRIBUTES   stObject;
    IO_STATUS_BLOCK     stIOStatus;
    NTSTATUS            lStatus;
    HANDLE              hFile = NULL;
    if (NT_InitPathObject(&stObject, FileName, RootDirectory, &stString)) {
        lStatus = NtCreateFile(&hFile, DesiredAccess, &stObject, &stIOStatus, NULL, FILE_ATTRIBUTE_NORMAL, ShareAccess, CreateDisposition, CreateOptions, NULL, 0);
        Mem_HeapFree(stString.Buffer);
        if (!NT_SUCCESS(lStatus)) {
            EH_SetLastStatus(lStatus);
        }
    }
    return hFile;
}

_Success_(return != FALSE)
BOOL NTAPI File_GetSize(_In_ HANDLE FileHandle, _Out_ PSIZE_T Size) {
    IO_STATUS_BLOCK stIOStatus;
    FILE_STANDARD_INFORMATION fsi;
    NTSTATUS        lStatus;
    lStatus = NtQueryInformationFile(FileHandle, &stIOStatus, &fsi, sizeof(fsi), FileStandardInformation);
    if (NT_SUCCESS(lStatus)) {
        *Size = (SIZE_T)fsi.EndOfFile.QuadPart;
        return TRUE;
    } else {
        EH_SetLastStatus(lStatus);
        return FALSE;
    }
}

_Success_(return > 0)
ULONG NTAPI File_Read(_In_ HANDLE FileHandle, _Out_writes_bytes_(BytesToRead) PVOID Buffer, _In_ ULONG BytesToRead, PLARGE_INTEGER ByteOffset) {
    IO_STATUS_BLOCK stIOStatus;
    NTSTATUS        lStatus;
    lStatus = NtReadFile(FileHandle, NULL, NULL, NULL, &stIOStatus, Buffer, BytesToRead, ByteOffset, NULL);
    if (NT_SUCCESS(lStatus)) {
        return (ULONG)stIOStatus.Information;
    } else {
        EH_SetLastStatus(lStatus);
        return FALSE;
    }
}

_Success_(return != FALSE)
BOOL NTAPI File_IsDirectory(_In_z_ PCWSTR FilePath, _Out_ PBOOL Result) {
    UNICODE_STRING                  stString;
    OBJECT_ATTRIBUTES               stObjectAttr;
    FILE_NETWORK_OPEN_INFORMATION   stFileInfo;
    NTSTATUS                        lStatus;
    if (NT_InitPathObject(&stObjectAttr, FilePath, NULL, &stString)) {
        lStatus = NtQueryFullAttributesFile(&stObjectAttr, &stFileInfo);
        Mem_HeapFree(stString.Buffer);
        if (NT_SUCCESS(lStatus)) {
            *Result = stFileInfo.FileAttributes & FILE_ATTRIBUTE_DIRECTORY;
            return TRUE;
        }
    } else {
        lStatus = STATUS_OBJECT_NAME_INVALID;
    }
    EH_SetLastStatus(lStatus);
    return FALSE;
}

BOOL NTAPI File_Delete(_In_z_ PCWSTR FilePath) {
    UNICODE_STRING      stString;
    OBJECT_ATTRIBUTES   stObjectAttr;
    IO_STATUS_BLOCK     stIOStatus;
    HANDLE              hFile;
    NTSTATUS            lStatus;
    if (NT_InitPathObject(&stObjectAttr, FilePath, NULL, &stString)) {
        lStatus = NtCreateFile(&hFile, DELETE | SYNCHRONIZE, &stObjectAttr, &stIOStatus, NULL, 0, 0, FILE_OPEN, FILE_NON_DIRECTORY_FILE | FILE_SYNCHRONOUS_IO_NONALERT | FILE_DELETE_ON_CLOSE, NULL, 0);
        Mem_HeapFree(stString.Buffer);
        if (NT_SUCCESS(lStatus)) {
            NtClose(hFile);
            return TRUE;
        }
    } else {
        lStatus = STATUS_OBJECT_NAME_INVALID;
    }
    EH_SetLastStatus(lStatus);
    return FALSE;
}

BOOL NTAPI File_Dispose(HANDLE FileHandle) {
    IO_STATUS_BLOCK                 stIOStatus;
    FILE_DISPOSITION_INFORMATION    fdi = { TRUE };
    NTSTATUS                        lStatus;
    lStatus = NtSetInformationFile(FileHandle, &stIOStatus, &fdi, sizeof(fdi), FileDispositionInformation);
    if (NT_SUCCESS(lStatus)) {
        return TRUE;
    } else {
        EH_SetLastStatus(lStatus);
        return FALSE;
    }
}

BOOL NTAPI File_SetSize(HANDLE FileHandle, ULONGLONG NewSize) {
    IO_STATUS_BLOCK                 stIOStatus;
    FILE_END_OF_FILE_INFORMATION    feofi;
    NTSTATUS                        lStatus;
    feofi.EndOfFile.QuadPart = NewSize;
    lStatus = NtSetInformationFile(FileHandle, &stIOStatus, &feofi, sizeof(feofi), FileEndOfFileInformation);
    if (NT_SUCCESS(lStatus)) {
        return TRUE;
    } else {
        EH_SetLastStatus(lStatus);
        return FALSE;
    }
}

_Success_(return != FALSE)
BOOL NTAPI File_ReadOnlyMap(_In_z_ PCWSTR FileName, HANDLE RootDirectory, _Out_ PFILE_MAP FileMap) {
    BOOL bRet = FALSE;
    HANDLE hFile, hSection;
    SIZE_T sFileSize;
    hFile = File_Create(FileName, RootDirectory, FILE_GENERIC_READ, FILE_SHARE_READ, FILE_OPEN, FILE_NON_DIRECTORY_FILE | FILE_SYNCHRONOUS_IO_NONALERT);
    if (hFile) {
        if (File_GetSize(hFile, &sFileSize)) {
            NTSTATUS lStatus;
            lStatus = NtCreateSection(&hSection, SECTION_MAP_READ, NULL, NULL, PAGE_READONLY, SEC_COMMIT, hFile);
            if (NT_SUCCESS(lStatus)) {
                PVOID BaseAddress = NULL;
                SIZE_T sPageSize = sFileSize;
                lStatus = NtMapViewOfSection(hSection, CURRENT_PROCESS_HANDLE, &BaseAddress, 0, 0, 0, &sPageSize, ViewUnmap, 0, PAGE_READONLY);
                if (NT_SUCCESS(lStatus)) {
                    FileMap->FileHandle = hFile;
                    FileMap->SectionHandle = hSection;
                    FileMap->FileSize = sFileSize;
                    FileMap->Map = BaseAddress;
                    FileMap->MapSize = sPageSize;
                    return TRUE;
                }
                NtClose(hSection);
            }
            EH_SetLastStatus(lStatus);
        }
        NtClose(hFile);
    }
    return bRet;
}

_Success_(return != FALSE)
BOOL NTAPI File_WritableMap(_In_z_ PCWSTR FileName, HANDLE RootDirectory, _Out_ PFILE_MAP FileMap, BOOL UseCache, SIZE_T MaximumSize) {
    BOOL bRet = FALSE;
    HANDLE hFile, hSection;
    SIZE_T sFileSize;
    hFile = File_Create(FileName, RootDirectory, FILE_GENERIC_WRITE, FILE_SHARE_READ, FILE_OPEN, FILE_NON_DIRECTORY_FILE | FILE_SYNCHRONOUS_IO_NONALERT | (UseCache ? 0 : FILE_NO_INTERMEDIATE_BUFFERING));
    if (hFile) {
        if (File_GetSize(hFile, &sFileSize)) {
            NTSTATUS lStatus;
            LARGE_INTEGER liMaximumSize;
            lStatus = NtCreateSection(
                &hSection,
                SECTION_MAP_WRITE,
                NULL,
                MaximumSize ? (liMaximumSize.QuadPart = MaximumSize, &liMaximumSize) : NULL,
                PAGE_READONLY,
                SEC_COMMIT | (UseCache ? 0 : SEC_NOCACHE),
                hFile);
            if (NT_SUCCESS(lStatus)) {
                PVOID BaseAddress = NULL;
                SIZE_T sPageSize = MaximumSize ? MaximumSize : sFileSize;
                lStatus = NtMapViewOfSection(hSection, CURRENT_PROCESS_HANDLE, &BaseAddress, 0, 0, 0, &sPageSize, ViewUnmap, 0, PAGE_READWRITE);
                if (NT_SUCCESS(lStatus)) {
                    FileMap->FileHandle = hFile;
                    FileMap->SectionHandle = hSection;
                    FileMap->FileSize = sFileSize;
                    FileMap->Map = BaseAddress;
                    FileMap->MapSize = sPageSize;
                    return TRUE;
                }
                NtClose(hSection);
            }
            EH_SetLastStatus(lStatus);
        }
        NtClose(hFile);
    }
    return bRet;
}

_Success_(return != FALSE)
BOOL NTAPI File_Map(_In_z_ PCWSTR FileName, HANDLE RootDirectory, _Out_ PFILE_MAP FileMap, ULONGLONG MaximumSize, ACCESS_MASK DesiredAccess, ULONG ShareAccess, ULONG CreateDisposition, BOOL NoCache, SECTION_INHERIT InheritDisposition) {
    ULONG           CreateOptions;
    HANDLE          hFile;
    HANDLE          hSection;
    PVOID           BaseAddress;
    ULONG           PageProtection;
    ULONG           AllocationAttributes;
    LARGE_INTEGER   stMaxSize;
    NTSTATUS        lStatus;
    CreateOptions = NoCache ? FILE_NON_DIRECTORY_FILE | FILE_SYNCHRONOUS_IO_NONALERT | FILE_NO_INTERMEDIATE_BUFFERING : FILE_NON_DIRECTORY_FILE | FILE_SYNCHRONOUS_IO_NONALERT;
    hFile = File_Create(FileName, RootDirectory, DesiredAccess, ShareAccess, CreateDisposition, CreateOptions);
    if (hFile) {
        stMaxSize.QuadPart = MaximumSize;
        PageProtection = DesiredAccess & FILE_WRITE_DATA ? PAGE_READWRITE : PAGE_READONLY;
        AllocationAttributes = NoCache ? SEC_COMMIT | SEC_NOCACHE : SEC_COMMIT;
        lStatus = NtCreateSection(&hSection, SECTION_ALL_ACCESS, NULL, &stMaxSize, PageProtection, AllocationAttributes, hFile);
        if (NT_SUCCESS(lStatus)) {
            BaseAddress = NULL;
            lStatus = NtMapViewOfSection(hSection, CURRENT_PROCESS_HANDLE, &BaseAddress, 0, 0, 0, (PSIZE_T)&stMaxSize.QuadPart, InheritDisposition, 0, PageProtection);
            if (NT_SUCCESS(lStatus)) {
                FileMap->FileHandle = hFile;
                FileMap->SectionHandle = hSection;
                FileMap->Map = BaseAddress;
                FileMap->MapSize = (SIZE_T)stMaxSize.QuadPart;
            } else {
                NtClose(hSection);
                NtClose(hFile);
            }
        } else
            NtClose(hFile);
        if (NT_SUCCESS(lStatus)) {
            return TRUE;
        } else {
            EH_SetLastStatus(lStatus);
            return FALSE;
        }
    } else {
        return FALSE;
    }
}

VOID NTAPI File_Unmap(_In_ PFILE_MAP FileMap) {
    NtUnmapViewOfSection(CURRENT_PROCESS_HANDLE, FileMap->Map);
    NtClose(FileMap->SectionHandle);
    NtClose(FileMap->FileHandle);
}
