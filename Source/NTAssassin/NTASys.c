#include "include\NTAssassin\NTASys.h"
#include "include\NTAssassin\NTANT.h"
#include "include\NTAssassin\NTAProc.h"
#include "include\NTAssassin\NTAMem.h"

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

HMODULE NTAPI Sys_LoadDll(SYS_DLL_NAME SysDll) {
    if (SysDll >= 0 && SysDll < SysDllNameMax) {
        if (!hSysDlls[SysDll]) {
            hSysDlls[SysDll] = SysDll == SysDllNameNTDll ? NT_GetNtdllHandle() : Proc_LoadDll(pszSysDllNames[SysDll - 1], FALSE);
        }
        return hSysDlls[SysDll];
    } else {
        return NULL;
    }
}

PVOID NTAPI Sys_LoadAPI(SYS_DLL_NAME SysDll, _In_z_ PCSTR APIName) {
    HMODULE hDll = Sys_LoadDll(SysDll);
    return hDll ? Proc_GetProcAddr(hDll, APIName) : NULL;
}

PCWSTR NTAPI Sys_GetMessage(HMODULE ModuleHandle, DWORD MessageId) {
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

PSYSTEM_PROCESS_INFORMATION NTAPI Sys_GetProcessInfo() {
    ULONG ulSize = 0;
    PSYSTEM_PROCESS_INFORMATION pSPI = NULL;
    NtQuerySystemInformation(SystemProcessInformation, NULL, 0, &ulSize);
    if (ulSize) {
        pSPI = Mem_Alloc(ulSize);
        if (pSPI && !NT_SUCCESS(NtQuerySystemInformation(SystemProcessInformation, pSPI, ulSize, NULL))) {
            Mem_Free(pSPI);
            pSPI = NULL;
        }
    }
    return pSPI;
}

BOOL NTAPI Sys_EqualGUID(REFGUID GUID1, REFGUID GUID2) {
    return GUID1->Data1 == GUID2->Data1 &&
        GUID1->Data2 == GUID2->Data2 &&
        GUID1->Data3 == GUID2->Data3 &&
        *(PQWORD)(GUID1->Data4) == *(PQWORD)(GUID2->Data4);
}

// WIP

#include "include\NTAssassin\NTAStr.h"

#define SYS_REG_SERVICES_PATH L"\\Registry\\Machine\\SYSTEM\\CurrentControlSet\\Services"

BOOL NTAPI Sys_RegDriver(PCWSTR Name, PCWSTR ImagePath, BOOL Volatile) {
    HANDLE              hKey;
    WCHAR               szRegSvc[MAX_REG_KEYNAME_CCH];
    UNICODE_STRING      strRegSvc, strKeyName;
    OBJECT_ATTRIBUTES   obRegSvc;
    ULONG               ulDisposition;
    NTSTATUS            lStatus;
    BOOL                bRet = FALSE;

    SIZE_T sIndex = Str_CopyW(szRegSvc, SYS_REG_SERVICES_PATH);
    szRegSvc[sIndex++] = '\\';
    sIndex += Str_CopyExW(szRegSvc + sIndex, ARRAYSIZE(szRegSvc) - sIndex, Name);
    if (sIndex > ARRAYSIZE(SYS_REG_SERVICES_PATH) && sIndex < ARRAYSIZE(szRegSvc)) {
        Str_InitW(&strRegSvc, szRegSvc);
        NT_InitObject(&obRegSvc, NULL, &strRegSvc, OBJ_CASE_INSENSITIVE | OBJ_KERNEL_HANDLE);
        lStatus = NtCreateKey(&hKey, KEY_SET_VALUE, &obRegSvc, 0, NULL, Volatile ? REG_OPTION_VOLATILE : REG_OPTION_NON_VOLATILE, &ulDisposition);
        if (NT_SUCCESS(lStatus)) {
            Str_InitW(&strKeyName, (PWSTR)L"ImagePath");
            lStatus = NtSetValueKey(hKey, &strKeyName, 0, REG_EXPAND_SZ, (PVOID)ImagePath, (ULONG)(Str_SizeW(ImagePath) + sizeof(WCHAR)));
            if (!NT_SUCCESS(lStatus)) {
                goto Label_1;
            }
            Str_InitW(&strKeyName, L"Type");
            DWORD dwType = SERVICE_KERNEL_DRIVER;
            lStatus = NtSetValueKey(hKey, &strKeyName, 0, REG_DWORD, &dwType, sizeof(dwType));
            if (!NT_SUCCESS(lStatus))
                goto Label_1;
            bRet = TRUE;
            goto Label_0;
        Label_1:
            if (ulDisposition == REG_CREATED_NEW_KEY) {
                NtDeleteKey(hKey);
            }
        Label_0:
            NtClose(hKey);
        }
    }
    return bRet;
}
