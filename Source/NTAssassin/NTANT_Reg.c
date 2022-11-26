#include "include\NTAssassin\NTANT.h"
#include "include\NTAssassin\NTAEH.h"

HANDLE NTAPI NT_RegOpenKey(_In_ PUNICODE_STRING KeyPath, _In_ ACCESS_MASK DesiredAccess)
{
    HANDLE hKey;
    OBJECT_ATTRIBUTES RegObj = RTL_CONSTANT_OBJECT_ATTRIBUTES(KeyPath, OBJ_CASE_INSENSITIVE | OBJ_KERNEL_HANDLE);
    NTSTATUS Status = NtOpenKey(&hKey, DesiredAccess, &RegObj);
    if (NT_SUCCESS(Status)) {
        return hKey;
    } else {
        EH_SetLastStatus(Status);
        return NULL;
    }
}

_Success_(return != FALSE)
BOOL NTAPI NT_RegGetDword(_In_ HANDLE KeyHandle, _In_ PUNICODE_STRING KeyName, _Out_ PDWORD Value)
{
    BOOL Ret = FALSE;
    DEFINE_ANYSIZE_STRUCT(ValueInfo, KEY_VALUE_PARTIAL_INFORMATION, UCHAR, sizeof(DWORD));
    ULONG Length;
    NTSTATUS Status = NtQueryValueKey(KeyHandle, KeyName, KeyValuePartialInformation, &ValueInfo, sizeof(ValueInfo), &Length);
    if (NT_SUCCESS(Status)) {
        if (ValueInfo.Base.Type == REG_DWORD || ValueInfo.Base.Type == REG_DWORD_BIG_ENDIAN) {
            *Value = *(DWORD*)(ValueInfo.Base.Data);
            Ret = TRUE;
        } else {
            Status = STATUS_DATATYPE_MISALIGNMENT;
        }
    }
    if (!Ret) {
        EH_SetLastStatus(Status);
    }
    return Ret;
}
