#include "Include\NTASys.h"

#include "Include\NTAProc.h"

static HMODULE hSysDlls[SysDllNameMax] = { NULL };
static PCWSTR  pszSysDllNames[] = {
    // ntdll.dll always is the first module initialized
    L"kernel32.dll",
    L"user32.dll",
    L"Comdlg32.dll",
    L"Ole32.dll",
    L"UxTheme.dll",
    L"Dwmapi.dll",
    L"Shcore.dll",
    L"Ws2_32.dll",
    L"Winmm.dll"
};

HMODULE NTAPI Sys_LoadDll(SYS_DLL_NAME SysDll)
{
    if (SysDll >= 0 && SysDll < SysDllNameMax) {
        if (!hSysDlls[SysDll]) {
            hSysDlls[SysDll] = SysDll == SysDllNameNTDll ? CURRENT_NTDLL_BASE : Proc_LoadDll(pszSysDllNames[SysDll - 1], FALSE);
        }
        return hSysDlls[SysDll];
    } else {
        return NULL;
    }
}

PVOID NTAPI Sys_LoadAPI(SYS_DLL_NAME SysDll, _In_z_ PCSTR APIName)
{
    HMODULE hDll = Sys_LoadDll(SysDll);
    return hDll ? Proc_GetProcAddr(hDll, APIName) : NULL;
}

PCWSTR NTAPI Sys_GetMessage(HMODULE ModuleHandle, DWORD MessageId)
{
    PMESSAGE_RESOURCE_ENTRY lpstMRE;
    return NT_SUCCESS(
        RtlFindMessage(
            ModuleHandle,
            (ULONG)(ULONG_PTR)RT_MESSAGETABLE,
            0,
            MessageId,
            &lpstMRE)) &&
        lpstMRE->Flags & MESSAGE_RESOURCE_UNICODE ?
        (PCWSTR)lpstMRE->Text :
        NULL;
}

BOOL NTAPI Sys_EqualGUID(REFGUID GUID1, REFGUID GUID2)
{
    return GUID1->Data1 == GUID2->Data1 &&
        GUID1->Data2 == GUID2->Data2 &&
        GUID1->Data3 == GUID2->Data3 &&
        *(PQWORD)(GUID1->Data4) == *(PQWORD)(GUID2->Data4);
}
