#include "Include\NTAShell.h"

#include <shellapi.h>
#include <Shlobj.h>

#include "Include\NTANT.h"
#include "Include\NTAStr.h"
#include "Include\NTAEH.h"

BOOL NTAPI Shell_Locate(_In_ PCWSTR Path)
{
    WCHAR               szFile[MAX_PATH];
    SFGAOF              dwAttr;
    PIDLIST_ABSOLUTE    pidlDir, pidlFile;
    UINT                uIndexSlash;
    HRESULT             hr;
    DWORD               dwError;
    uIndexSlash = (UINT)Str_CopyW(szFile, Path);
    while (uIndexSlash > 0 && szFile[--uIndexSlash] != L'\\');
    if (uIndexSlash > 0)
    {
        szFile[uIndexSlash] = UNICODE_NULL;
        hr = SHParseDisplayName(szFile, NULL, &pidlDir, 0, &dwAttr);
        if (hr == S_OK)
        {
            szFile[uIndexSlash] = L'\\';
            hr = SHParseDisplayName(szFile, NULL, &pidlFile, 0, &dwAttr);
            if (hr == S_OK)
            {
                hr = SHOpenFolderAndSelectItems(pidlDir, 1, &pidlFile, 0);
                CoTaskMemFree(pidlFile);
                if (hr == S_OK)
                {
                    CoTaskMemFree(pidlDir);
                    return TRUE;
                }
            }
            CoTaskMemFree(pidlDir);
        }
        dwError = EH_HrToWin32(hr);
    } else
    {
        dwError = ERROR_BAD_PATHNAME;
    }
    WIE_SetLastError(dwError);
    return FALSE;
}

BOOL NTAPI Shell_Exec(_In_ PCWSTR File, _In_opt_ PCWSTR Param, PCWSTR Verb, INT ShowCmd, PHANDLE ProcessHandle)
{
    SHELLEXECUTEINFOW   stSEIW = { sizeof(SHELLEXECUTEINFOW) };
    BOOL                bRet;
    stSEIW.fMask = (ProcessHandle ? SEE_MASK_NOCLOSEPROCESS : SEE_MASK_DEFAULT) | SEE_MASK_INVOKEIDLIST;
    stSEIW.lpFile = File;
    stSEIW.lpParameters = Param;
    stSEIW.lpVerb = Verb;
    stSEIW.nShow = ShowCmd;
    bRet = ShellExecuteExW(&stSEIW);
    if (bRet && ProcessHandle)
    {
        *ProcessHandle = stSEIW.hProcess;
    }
    return bRet;
}

_Success_(return != FALSE)
BOOL NTAPI Shell_GetLinkPath(_In_ PCWSTR LinkFile, _Out_writes_z_(PathCchSize) PWSTR Path, _In_ INT PathCchSize)
{
    BOOL bRet = FALSE;
    IShellLinkW* psl;
    HRESULT hr = CoCreateInstance(&CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER, &IID_IShellLinkW, &psl);
    if (SUCCEEDED(hr))
    {
        IPersistFile* ppf;
        hr = psl->lpVtbl->QueryInterface(psl, &IID_IPersistFile, &ppf);
        if (SUCCEEDED(hr))
        {
            hr = ppf->lpVtbl->Load(ppf, LinkFile, STGM_READ);
            if (SUCCEEDED(hr))
            {
                hr = psl->lpVtbl->GetPath(psl, Path, PathCchSize, NULL, 0);
                bRet = SUCCEEDED(hr);
            }
            ppf->lpVtbl->Release(ppf);
        }
        psl->lpVtbl->Release(psl);
    }
    if (!bRet)
    {
        WIE_SetLastError(EH_HrToWin32(hr));
    }
    return bRet;
}
