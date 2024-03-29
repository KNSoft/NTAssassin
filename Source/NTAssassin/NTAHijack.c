﻿#include "Include\NTAHijack.h"

#include "Include\NTAStr.h"
#include "Include\NTARProc.h"
#include "Include\NTAProc.h"

#include "Precomp4C.Output.h"

//  [STRING_ALIGNMENT]
//  UINT    uSize;          // OUT The final size of stucture
//  WCHAR   szLibName[];    // IN Terminated by '\0'
//  WORD    wProcOrdinal;   // IN OPTIONAL, Ordinal of procedure, 0 if szProcName presents
//  [STRING_ALIGNMENT]
//  CHAR    szProcName[];   // IN OPTIONAL, Terminated by '\0', exists only if wProcOrdinal is 0
                            // Could be empty string if load DLL only
//  PVOID64 pProc;          // OUT OPTIONAL, Receive address of the procedure
#define HIJACK_LOADPROCADDR_MAXPARAMBUFFERSIZE (STRING_ALIGNMENT + sizeof(UINT) + MAX_PATH * sizeof(WCHAR) + sizeof(WORD) + STRING_ALIGNMENT + MAX_CIDENTIFIERNAME_CCH + sizeof(LPVOID))

// Fixme: Patches C6101, in fact, beginning bytes unaligned will be ignored
#pragma warning(disable: 6101)
_Ret_notnull_
PVOID NTAPI Hijack_LoadProcAddr_InitParam(_Out_writes_bytes_(HIJACK_LOADPROCADDR_MAXPARAMBUFFERSIZE) PVOID Buffer, _In_z_ PCWSTR LibName, _In_opt_z_ PCSTR ProcName, _Out_opt_ _When_(ProcName != NULL, _Notnull_) PVOID64** ProcAddrPointer)
{
    SIZE_T  iCcb;
    WORD    wProcOrdinal;
    PVOID   pStruct, pTemp;
    PDWORD  pdwSize;
    pTemp = pStruct = (PVOID)ROUND_TO_SIZE((ULONG_PTR)Buffer, STRING_ALIGNMENT);

    // uSize
    pdwSize = pTemp;
    pTemp = Add2Ptr(pTemp, sizeof(UINT));

    // szLibName
    iCcb = Str_CopyExW((PWSTR)pTemp, MAX_PATH, LibName) << 1;
    pTemp = Add2Ptr(pTemp, iCcb + sizeof(WCHAR));

    // wProcOrdinal
    wProcOrdinal = (UINT_PTR)ProcName <= MAXWORD ? LOWORD(ProcName) : 0;
    *(PWORD)pTemp = wProcOrdinal;
    pTemp = Add2Ptr(pTemp, sizeof(WORD));

    // szProcName
    if (!wProcOrdinal)
    {
        pTemp = (PVOID)ROUND_TO_SIZE((ULONG_PTR)pTemp, STRING_ALIGNMENT);
        if (ProcName)
        {
// Fixme: Patches C6387, in fact, ProcName won't be NULL here
#pragma warning(disable: 6387)
            iCcb = Str_CopyExA((PSTR)pTemp, MAX_CIDENTIFIERNAME_CCH, ProcName);
#pragma warning(default: 6387)
        } else
        {
            ((PSTR)pTemp)[0] = '\0';
            iCcb = 0;
        }
        pTemp = Add2Ptr(pTemp, iCcb + sizeof(CHAR));
    }

    // pProc
    if (ProcName)
    {
        *(PVOID64*)pTemp = NULL;
        *ProcAddrPointer = (PVOID64*)pTemp;
        pTemp = Add2Ptr(pTemp, sizeof(PVOID64));
    }

    // Done
    *pdwSize = (DWORD)((DWORD_PTR)pTemp - (DWORD_PTR)Buffer);
    return pStruct;
}
#pragma warning(default: 6101)

_Success_(return != FALSE)
BOOL NTAPI Hijack_ExecShellcode(_In_ HANDLE ProcessHandle, _In_reads_bytes_(ShellCodeSize) PVOID ShellCode, SIZE_T ShellCodeSize, _In_reads_bytes_opt_(ParamSize) PVOID Param, SIZE_T ParamSize, _Out_opt_ PDWORD ExitCode, DWORD Timeout)
{
    BOOL        bRet = FALSE;
    RPROC_MAP   ProcMap, ParamMap;
    PVOID       pParam;
    HANDLE      hThread;
    BOOL        bKeepMem = FALSE;

    // Maps shellcode and parameter
    RProc_InitMap(&ProcMap, ShellCode, ShellCodeSize, NULL, PAGE_EXECUTE_READ);
    if (!RProc_MemMap(ProcessHandle, &ProcMap))
    {
        goto Label_0;
    }
    if (Param)
    {
        RProc_InitMap(&ParamMap, Param, ParamSize, NULL, PAGE_READWRITE);
        if (!RProc_MemMap(ProcessHandle, &ParamMap))
        {
            goto Label_1;
        }
        pParam = ParamMap.Remote;
    } else
    {
        pParam = NULL;
    }

    // Create remote thread and wait
    if (!RProc_CreateThread(ProcessHandle, ProcMap.Remote, pParam, FALSE, &hThread))
    {
        goto Label_2;
    }

    if (Timeout)
    {
        NTSTATUS lStatus = Proc_WaitForObject(hThread, Timeout);
        if (lStatus != STATUS_SUCCESS)
        {
// Keep remote memory due to the thread still running
            WIE_SetLastStatus(lStatus);
            bKeepMem = TRUE;
            goto Label_3;
        }

        // Receive return values and parameters
        if (ExitCode)
        {
            if (!Proc_GetThreadExitCode(hThread, ExitCode))
            {
                goto Label_3;
            }
        }

        if (!pParam || RProc_ReadMemory(ProcessHandle, pParam, Param, ParamSize))
        {
            bRet = TRUE;
        }
    } else
    {
        if (ExitCode)
        {
            *ExitCode = STILL_ACTIVE;
        }
        bRet = TRUE;
    }

    // Cleanup and exit
Label_3:
    NtClose(hThread);
Label_2:
    if (!bKeepMem && pParam)
    {
        RProc_MemUnmap(ProcessHandle, &ParamMap);
    }
Label_1:
    if (!bKeepMem)
    {
        RProc_MemUnmap(ProcessHandle, &ProcMap);
    }
Label_0:
    return bRet;
}

_Success_(return != FALSE)
BOOL NTAPI Hijack_LoadProcAddr(_In_ HANDLE ProcessHandle, _In_z_ PCWSTR LibName, _In_opt_z_ PCSTR ProcName, _Out_opt_ _When_(ProcName != NULL, _Notnull_) PVOID64 * ProcAddr, DWORD Timeout)
{
    BYTE    Buffer[HIJACK_LOADPROCADDR_MAXPARAMBUFFERSIZE];
    LPVOID  pParam;
    PVOID64 *pProc;
    DWORD   ExitCode;
    BOOL    b32Proc;
#ifdef _WIN64
    if (!RProc_IsWow64(ProcessHandle, &b32Proc))
    {
        return FALSE;
    }
#else
    b32Proc = TRUE;
#endif
    // Initialize parameter
    pParam = Hijack_LoadProcAddr_InitParam(Buffer, LibName, ProcName, &pProc);
    // Create remote thread and execute
    if (Hijack_ExecShellcode(
        ProcessHandle,
        b32Proc ? SYM_Hijack_LoadProcAddr_InjectThread_x86 : SYM_Hijack_LoadProcAddr_InjectThread_x64,
        b32Proc ? sizeof(SYM_Hijack_LoadProcAddr_InjectThread_x86) : sizeof(SYM_Hijack_LoadProcAddr_InjectThread_x64),
        pParam,
        *(PDWORD)pParam,
        &ExitCode,
        Timeout))
    {
        if (NT_SUCCESS(ExitCode))
        {
            if (ProcAddr)
            {
                *ProcAddr = *pProc;
            }
            return TRUE;
        } else
        {
            WIE_SetLastStatus(ExitCode);
        }
    }
    return FALSE;
}

// TODO 重写
BOOL NTAPI Hijack_CallProc(_In_ HANDLE ProcessHandle, _Inout_ PHIJACK_CALLPROCHEADER CallProcHeader, _In_opt_ PHIJACK_CALLPROCPARAM Params, DWORD Timeout)
{
    BOOL                    bRet = FALSE;
    NTSTATUS                lStatus;
    SIZE_T                  usTotalSize, usPageSize;
    UINT                    i, uParamCount;
    HIJACK_CALLPROCHEADER   stCallProcRet;
    HIJACK_CALLPROCPARAM*   pParam, *pRemoteParam, stMemParam;
    PVOID                   pRemoteBuffer, pRemoteRandomParam, pTemp;
    RPROC_MAP               ProcMap;
    HANDLE                  hThread;
    BOOL                    b32Proc;
    BOOL                    bKeepMem = FALSE;

#ifdef _WIN64
    if (!RProc_IsWow64(ProcessHandle, &b32Proc))
    {
        return FALSE;
    }
#else
    b32Proc = TRUE;
#endif

    // Calculate size of remote buffer and allocate memory
    uParamCount = CallProcHeader->ParamCount;
    usTotalSize = sizeof(HIJACK_CALLPROCHEADER) + sizeof(HIJACK_CALLPROCPARAM) * uParamCount;
    pParam = Params;
    for (i = 0; i < uParamCount; i++)
    {
        _Analysis_assume_(pParam != NULL);
        usTotalSize += ROUND_TO_SIZE(pParam[i].Size, STRING_ALIGNMENT);
    }
    pRemoteBuffer = NULL;
    usPageSize = usTotalSize;
    lStatus = NtAllocateVirtualMemory(ProcessHandle, &pRemoteBuffer, 0, &usPageSize, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    if (!NT_SUCCESS(lStatus))
    {
        WIE_SetLastStatus(lStatus);
        goto Label_0;
    }

    // Write buffer:
    // HIJACK_CALLPROCHEADER
    // MEMORY_RANGE_ENTRY[HIJACK_CALLPROCHEADER.ParamCount]
    // Random Parameters..., align to STRING_ALIGNMENT
    lStatus = NtWriteVirtualMemory(ProcessHandle, pRemoteBuffer, CallProcHeader, sizeof(HIJACK_CALLPROCHEADER), NULL);
    if (!NT_SUCCESS(lStatus))
    {
        WIE_SetLastStatus(lStatus);
        goto Label_1;
    }
    pRemoteParam = Add2Ptr(pRemoteBuffer, sizeof(HIJACK_CALLPROCHEADER));
    pTemp = pRemoteRandomParam = Add2Ptr(pRemoteParam, sizeof(HIJACK_CALLPROCPARAM) * uParamCount);
    for (i = uParamCount; i > 0; i--)
    {
        stMemParam.Size = pParam[i - 1].Size;
        if (stMemParam.Size)
        {
            lStatus = NtWriteVirtualMemory(ProcessHandle, pTemp, (PVOID)pParam[i - 1].Address, (SIZE_T)stMemParam.Size, NULL);
            if (!NT_SUCCESS(lStatus))
            {
                WIE_SetLastStatus(lStatus);
                goto Label_1;
            }
            stMemParam.Address = pTemp;
            pTemp = Add2Ptr(pTemp, ROUND_TO_SIZE(stMemParam.Size, STRING_ALIGNMENT));
        } else
        {
            stMemParam.Address = pParam[i - 1].Address;
        }
        lStatus = NtWriteVirtualMemory(ProcessHandle, pRemoteParam, &stMemParam, sizeof(stMemParam), NULL);
        if (!NT_SUCCESS(lStatus))
        {
            WIE_SetLastStatus(lStatus);
            goto Label_1;
        }
        pRemoteParam++;
    }

    // Create remote thread
    RProc_InitMap(
        &ProcMap,
        b32Proc ? SYM_Hijack_CallProc_InjectThread_x86 : SYM_Hijack_CallProc_InjectThread_x64,
        b32Proc ? sizeof(SYM_Hijack_CallProc_InjectThread_x86) : sizeof(SYM_Hijack_CallProc_InjectThread_x64),
        NULL,
        PAGE_EXECUTE_READ);
    if (!RProc_MemMap(ProcessHandle, &ProcMap))
    {
        goto Label_1;
    }
    if (!RProc_CreateThread(ProcessHandle, ProcMap.Remote, pRemoteBuffer, FALSE, &hThread))
    {
        goto Label_2;
    }
    lStatus = Proc_WaitForObject(hThread, Timeout);
    if (lStatus != STATUS_SUCCESS)
    {
        bKeepMem = TRUE;
        WIE_SetLastStatus(lStatus);
        goto Label_3;
    }

    // Get return values and parameters
    lStatus = NtReadVirtualMemory(ProcessHandle, pRemoteBuffer, &stCallProcRet, sizeof(stCallProcRet), NULL);
    if (!NT_SUCCESS(lStatus))
    {
        WIE_SetLastStatus(lStatus);
        goto Label_3;
    }
    CallProcHeader->RetValue = stCallProcRet.RetValue;
    CallProcHeader->LastError = stCallProcRet.LastError;
    CallProcHeader->LastStatus = stCallProcRet.LastStatus;
    CallProcHeader->ExceptionCode = stCallProcRet.ExceptionCode;
    for (i = uParamCount; i > 0; i--)
    {
        if (pParam[i - 1].Size)
        {
            if (pParam[i - 1].Out)
            {
                lStatus = NtReadVirtualMemory(ProcessHandle, pRemoteRandomParam, (PVOID)pParam[i - 1].Address, (SIZE_T)pParam[i - 1].Size, NULL);
                if (!NT_SUCCESS(lStatus))
                {
                    WIE_SetLastStatus(lStatus);
                    goto Label_3;
                }
            }
            pRemoteRandomParam = Add2Ptr(pRemoteRandomParam, ROUND_TO_SIZE(pParam[i - 1].Size, STRING_ALIGNMENT));
        }
    }
    bRet = TRUE;

Label_3:
    NtClose(hThread);
Label_2:
    if (!bKeepMem)
    {
        RProc_MemUnmap(ProcessHandle, &ProcMap);
    }
Label_1:
    if (!bKeepMem)
    {
        NtFreeVirtualMemory(ProcessHandle, &pRemoteBuffer, &usPageSize, MEM_RELEASE);
    }
Label_0:
    return bRet;
}
