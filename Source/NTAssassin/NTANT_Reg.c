#include "Include\NTANT.h"

_Success_(return != FALSE)
BOOL NTAPI NT_RegGetDword(_In_ HANDLE KeyHandle, _In_ PUNICODE_STRING KeyName, _Out_ PDWORD Value)
{
    BOOL Ret = FALSE;
    DEFINE_ANYSIZE_STRUCT(ValueInfo, KEY_VALUE_PARTIAL_INFORMATION, UCHAR, sizeof(DWORD));
    ULONG Length;
    NTSTATUS Status = NtQueryValueKey(KeyHandle, KeyName, KeyValuePartialInformation, &ValueInfo, sizeof(ValueInfo), &Length);
    if (NT_SUCCESS(Status))
    {
        if (ValueInfo.Base.Type == REG_DWORD || ValueInfo.Base.Type == REG_DWORD_BIG_ENDIAN)
        {
            *Value = *(DWORD*)(ValueInfo.Base.Data);
            Ret = TRUE;
        } else
        {
            Status = STATUS_BUFFER_TOO_SMALL;
        }
    }
    if (!Ret)
    {
        WIE_SetLastStatus(Status);
    }
    return Ret;
}
