#include "include\NTAssassin\NTAssassin.h"

NTSTATUS NTAPI File_Create(PHANDLE FileHandle, PWSTR FileName, HANDLE RootDirectory, ACCESS_MASK DesiredAccess, ULONG ShareAccess, ULONG CreateDisposition, ULONG CreateOptions) {
    UNICODE_STRING      stString;
    OBJECT_ATTRIBUTES   stObjectAttr;
    IO_STATUS_BLOCK     stIOStatusBlock;
    NTSTATUS            lStatus;
    lStatus = NT_InitPathObject(FileName, RootDirectory ,&stObjectAttr, &stString);
    if (NT_SUCCESS(lStatus)) {
        lStatus = NtCreateFile(FileHandle, DesiredAccess, &stObjectAttr, &stIOStatusBlock, NULL, 0, ShareAccess, CreateDisposition, CreateOptions, NULL, 0);
        Mem_HeapFree(stString.Buffer);
    }
    return lStatus;
}

NTSTATUS NTAPI File_IsDirectory(PWSTR FilePath, PBOOL Result) {
    UNICODE_STRING                  stString;
    OBJECT_ATTRIBUTES               stObjectAttr;
    FILE_NETWORK_OPEN_INFORMATION   stFileInfo;
    NTSTATUS                        lStatus;
    lStatus = NT_InitPathObject(FilePath, NULL, &stObjectAttr, &stString);
    if (NT_SUCCESS(lStatus)) {
        lStatus = NtQueryFullAttributesFile(&stObjectAttr, &stFileInfo);
        if (NT_SUCCESS(lStatus))
            *Result = stFileInfo.FileAttributes & FILE_ATTRIBUTE_DIRECTORY;
        Mem_HeapFree(stString.Buffer);
    }
    return lStatus;
}

NTSTATUS NTAPI File_Delete(PWSTR FileName) {
    UNICODE_STRING      stString;
    OBJECT_ATTRIBUTES   stObjectAttr;
    IO_STATUS_BLOCK     stIOStatusBlock;
    HANDLE              hFile;
    NTSTATUS            lStatus;
    lStatus = NT_InitPathObject(FileName, NULL, &stObjectAttr, &stString);
    if (NT_SUCCESS(lStatus)) {
        lStatus = NtCreateFile(&hFile, DELETE | SYNCHRONIZE, &stObjectAttr, &stIOStatusBlock, NULL, 0, 0, FILE_OPEN, FILE_NON_DIRECTORY_FILE | FILE_SYNCHRONOUS_IO_NONALERT | FILE_DELETE_ON_CLOSE, NULL, 0);
        if (NT_SUCCESS(lStatus))
            lStatus = NtClose(hFile);
        Mem_HeapFree(stString.Buffer);
    }
    return lStatus;
}

NTSTATUS NTAPI File_Dispose(HANDLE FileHandle, BOOL Disposition) {
    IO_STATUS_BLOCK                 stIOStatusBlock;
    FILE_DISPOSITION_INFORMATION    fdi = { Disposition };
    return NtSetInformationFile(FileHandle, &stIOStatusBlock, &fdi, sizeof(fdi), FileDispositionInformation);
}

NTSTATUS NTAPI File_SetSize(HANDLE FileHandle, ULONGLONG NewSize) {
    IO_STATUS_BLOCK                 stIOStatusBlock;
    FILE_END_OF_FILE_INFORMATION    feofi;
    feofi.EndOfFile.QuadPart = NewSize;
    return NtSetInformationFile(FileHandle, &stIOStatusBlock, &feofi, sizeof(feofi), FileEndOfFileInformation);
}

NTSTATUS NTAPI File_Map(PWSTR FileName, HANDLE RootDirectory, PFILE_MAP FileMap, ULONGLONG MaximumSize, ACCESS_MASK DesiredAccess, ULONG ShareAccess, ULONG CreateDisposition, BOOL NoCache, SECTION_INHERIT InheritDisposition) {
    ULONG           CreateOptions;
    HANDLE          hFile;
    HANDLE          hSection;
    PVOID           BaseAddress;
    ULONG           PageProtection;
    ULONG           AllocationAttributes;
    LARGE_INTEGER   stMaxSize;
    NTSTATUS        lStatus;
    CreateOptions = NoCache ? FILE_NON_DIRECTORY_FILE | FILE_SYNCHRONOUS_IO_NONALERT | FILE_NO_INTERMEDIATE_BUFFERING : FILE_NON_DIRECTORY_FILE | FILE_SYNCHRONOUS_IO_NONALERT;
    lStatus = File_Create(&hFile, FileName, RootDirectory, DesiredAccess, ShareAccess, CreateDisposition, CreateOptions);
    if (NT_SUCCESS(lStatus)) {
        stMaxSize.QuadPart = MaximumSize;
        PageProtection = DesiredAccess & FILE_WRITE_DATA ? PAGE_READWRITE : PAGE_READONLY;
        AllocationAttributes = NoCache ? SEC_COMMIT | SEC_NOCACHE : SEC_COMMIT;
        lStatus = NtCreateSection(&hSection, SECTION_ALL_ACCESS, NULL, &stMaxSize, PageProtection, AllocationAttributes, hFile);
        if (NT_SUCCESS(lStatus)) {
            BaseAddress = NULL;
            lStatus = NtMapViewOfSection(hSection, CURRENT_PROCESS_HANDLE, &BaseAddress, 0, 0, 0, (PSIZE_T)&stMaxSize.QuadPart, InheritDisposition, 0, PageProtection);
            if (NT_SUCCESS(lStatus)) {
                FileMap->DesiredAccess = DesiredAccess;
                FileMap->FileHandle = hFile;
                FileMap->SectionHandle = hSection;
                FileMap->Mem.VirtualAddress = BaseAddress;
                FileMap->Mem.NumberOfBytes = (SIZE_T)stMaxSize.QuadPart;
            } else {
                NtClose(hSection);
                NtClose(hFile);
            }
        } else
            NtClose(hFile);
    }
    return lStatus;
}

NTSTATUS NTAPI File_Unmap(PFILE_MAP FileMap) {
    NTSTATUS lStatus;
    lStatus = NtUnmapViewOfSection(CURRENT_PROCESS_HANDLE, FileMap->Mem.VirtualAddress);
    if (NT_SUCCESS(lStatus)) {
        NtClose(FileMap->SectionHandle);
        NtClose(FileMap->FileHandle);
    }
    return lStatus;
}