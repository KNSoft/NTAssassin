#include "Include\NTANT.h"

#include "Include\NTAMem.h"

BOOL NTAPI NT_InitPathObject(POBJECT_ATTRIBUTES Object, _In_z_ PCWSTR Path, HANDLE RootDirectory, PUNICODE_STRING ObjectName)
{
    if (RtlDosPathNameToNtPathName_U(Path, ObjectName, NULL, NULL)) {
        NT_InitObject(Object, RootDirectory, ObjectName, OBJ_CASE_INSENSITIVE | OBJ_KERNEL_HANDLE);
        return TRUE;
    } else {
        return FALSE;
    }
}

PVOID NTAPI NT_GetSystemInfo(_In_ SYSTEM_INFORMATION_CLASS SystemInformationClass)
{
    PVOID Info;
    ULONG Length;
    NTSTATUS Status;

    Status = NtQuerySystemInformation(SystemInformationClass, NULL, 0, &Length);
    if (Status != STATUS_INFO_LENGTH_MISMATCH) {
        goto _Error;
    }

_Retry:
    Info = Mem_Alloc(Length);
    if (Info) {
        Status = NtQuerySystemInformation(SystemInformationClass, Info, Length, &Length);
        if (NT_SUCCESS(Status)) {
            return Info;
        } else if (Status == STATUS_INFO_LENGTH_MISMATCH) {
            Mem_Free(Info);
            goto _Retry;
        } else {
            goto _Error;
        }
    } else {
        return NULL;
    }

_Error:
    WIE_SetLastStatus(Status);
    return NULL;
}
