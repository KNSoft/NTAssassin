#include "include\NTAssassin\NTAssassin.h"

DWORD NTAPI NT_SetLastNTError(NTSTATUS Status) {
    DWORD dwErrCode;
    dwErrCode = RtlNtStatusToDosError(Status);
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
