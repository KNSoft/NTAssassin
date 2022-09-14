#include "include\NTAssassin\NTANT.h"
#include "include\NTAssassin\NTARProc.h"
#include "include\NTAssassin\NTAMem.h"

DWORD NTAPI NT_SetLastNTError(NTSTATUS Status) {
    DWORD dwErrCode;
    dwErrCode = RtlNtStatusToDosErrorNoTeb(Status);
    NT_SetLastError(dwErrCode);
    return dwErrCode;
}

VOID NTAPI NT_InitObject(_Out_ POBJECT_ATTRIBUTES Object, HANDLE RootDirectory, PUNICODE_STRING ObjectName, ULONG Attributes) {
    Object->Length = sizeof(OBJECT_ATTRIBUTES);
    Object->RootDirectory = RootDirectory;
    Object->ObjectName = ObjectName;
    Object->Attributes = Attributes;
    Object->SecurityDescriptor = Object->SecurityQualityOfService = NULL;
}

BOOL NTAPI NT_InitPathObject(POBJECT_ATTRIBUTES Object, _In_z_ PCWSTR Path, HANDLE RootDirectory, PUNICODE_STRING ObjectName) {
    if (RtlDosPathNameToNtPathName_U(Path, ObjectName, NULL, NULL)) {
        NT_InitObject(Object, RootDirectory, ObjectName, OBJ_CASE_INSENSITIVE | OBJ_KERNEL_HANDLE);
        return TRUE;
    } else {
        return FALSE;
    }
}

PVOID NTAPI NT_GetSystemInfo(_In_ SYSTEM_INFORMATION_CLASS SystemInformationClass) {
    PVOID Info = NULL;
    ULONG Length;
    NTSTATUS Status;
    Status = NtQuerySystemInformation(SystemInformationClass, NULL, 0, &Length);
    if (Status == STATUS_BUFFER_TOO_SMALL) {
        if (Length) {
            Info = Mem_Alloc(Length);
            if (Info) {
                Status = NtQuerySystemInformation(SystemInformationClass, Info, Length, &Length);
                if (!NT_SUCCESS(Status)) {
                    Mem_Free(Info);
                    Info = NULL;
                }
            } else {
                return NULL;
            }
        } else {
            Status = STATUS_NOT_FOUND;
        }
    }
    if (!Info) {
        NT_SetLastStatus(Status);
    }
    return Info;
}
