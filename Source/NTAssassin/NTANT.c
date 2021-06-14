#include "NTAssassin\NTAssassin.h"

VOID NTAPI NT_InitObject(POBJECT_ATTRIBUTES Object, HANDLE RootDirectory, PUNICODE_STRING ObjectName, ULONG Attributes) {
    Object->Length = sizeof(OBJECT_ATTRIBUTES);
    Object->RootDirectory = RootDirectory;
    Object->ObjectName = ObjectName;
    Object->Attributes = Attributes;
    Object->SecurityDescriptor = Object->SecurityQualityOfService = NULL;
}

NTSTATUS NTAPI NT_InitPathObject(PCWSTR Path, HANDLE RootDirectory, POBJECT_ATTRIBUTES Object, PUNICODE_STRING ObjectName) {
    if (RtlDosPathNameToNtPathName_U(Path, ObjectName, NULL, NULL)) {
        NT_InitObject(Object, RootDirectory, ObjectName, OBJ_CASE_INSENSITIVE | OBJ_KERNEL_HANDLE);
        return STATUS_SUCCESS;
    } else
        return STATUS_OBJECT_NAME_INVALID;
}

int NT_SEH_NopHandler(LPEXCEPTION_POINTERS ExceptionInfo) {
    return EXCEPTION_EXECUTE_HANDLER;
}