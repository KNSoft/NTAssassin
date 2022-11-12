#include "include\NTAssassin\NTAShell.h"

#include <Shlobj.h>

#include "include\NTAssassin\NTANT.h"
#include "include\NTAssassin\NTAStr.h"
#include "include\NTAssassin\NTAEH.h"

BOOL NTAPI Shell_Locate(_In_ PCWSTR Path) {
    WCHAR               szFile[MAX_PATH];
    SFGAOF              dwAttr;
    PIDLIST_ABSOLUTE    pidlDir, pidlFile;
    UINT                uIndexSlash;
    HRESULT             hr;
    DWORD               dwError;
    uIndexSlash = (UINT)Str_CopyW(szFile, Path);
    while (uIndexSlash > 0 && szFile[--uIndexSlash] != L'\\');
    if (uIndexSlash > 0) {
        szFile[uIndexSlash] = L'\0';
        hr = SHParseDisplayName(szFile, NULL, &pidlDir, 0, &dwAttr);
        if (hr == S_OK) {
            szFile[uIndexSlash] = L'\\';
            hr = SHParseDisplayName(szFile, NULL, &pidlFile, 0, &dwAttr);
            if (hr == S_OK) {
                hr = SHOpenFolderAndSelectItems(pidlDir, 1, &pidlFile, 0);
                CoTaskMemFree(pidlFile);
                if (hr == S_OK) {
                    CoTaskMemFree(pidlDir);
                    return TRUE;
                }
            }
            CoTaskMemFree(pidlDir);
        }
        dwError = EH_HrToWin32(hr);
    } else {
        dwError = ERROR_BAD_PATHNAME;
    }
    EH_SetLastError(dwError);
    return FALSE;
}

BOOL NTAPI Shell_Exec(_In_ PCWSTR File, _In_opt_ PCWSTR Param, SHELL_EXEC_VERB Verb, INT ShowCmd, PHANDLE ProcessHandle) {
    SHELLEXECUTEINFOW   stSEIW = { sizeof(SHELLEXECUTEINFOW) };
    BOOL                bRet;
    stSEIW.fMask = (ProcessHandle ? SEE_MASK_NOCLOSEPROCESS : SEE_MASK_DEFAULT) | SEE_MASK_INVOKEIDLIST;
    stSEIW.lpFile = File;
    stSEIW.lpParameters = Param;
    if (Verb == ShellExecOpen) {
        stSEIW.lpVerb = L"open";
    } else if (Verb == ShellExecExplore) {
        stSEIW.lpVerb = L"explore";
    } else if (Verb == ShellExecRunAs) {
        stSEIW.lpVerb = L"runas";
    } else if (Verb == ShellExecProperties) {
        stSEIW.lpVerb = L"properties";
    } else if (Verb == ShellExecEdit) {
        stSEIW.lpVerb = L"edit";
    } else if (Verb == ShellExecPrint) {
        stSEIW.lpVerb = L"print";
    } else if (Verb == ShellExecFind) {
        stSEIW.lpVerb = L"find";
    } else {
        return FALSE;
    }
    stSEIW.nShow = ShowCmd;
    bRet = ShellExecuteExW(&stSEIW);
    if (bRet && ProcessHandle) {
        *ProcessHandle = stSEIW.hProcess;
    }
    return bRet;
}

_Success_(return != FALSE)
BOOL NTAPI Shell_GetLinkPath(_In_ PCWSTR LinkFile, _Out_writes_z_(PathCchSize) PWSTR Path, _In_ INT PathCchSize) {
    BOOL bRet = FALSE;
    IShellLinkW* psl;
    HRESULT hr = CoCreateInstance(&CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER, &IID_IShellLinkW, &psl);
    if (SUCCEEDED(hr)) {
        IPersistFile* ppf;
        hr = psl->lpVtbl->QueryInterface(psl, &IID_IPersistFile, &ppf);
        if (SUCCEEDED(hr)) {
            hr = ppf->lpVtbl->Load(ppf, LinkFile, STGM_READ);
            if (SUCCEEDED(hr)) {
                hr = psl->lpVtbl->GetPath(psl, Path, PathCchSize, NULL, 0);
                bRet = SUCCEEDED(hr);
            }
            ppf->lpVtbl->Release(ppf);
        }
        psl->lpVtbl->Release(psl);
    }
    if (!bRet) {
        EH_SetLastError(EH_HrToWin32(hr));
    }
    return bRet;
}
