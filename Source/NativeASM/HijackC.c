#include "..\NTAssassin\include\NTAssassin\NTAssassin.h"

NTSTATUS WINAPI Hijack_LoadProcAddr_InjectThread(LPVOID lParam) {
    PVOID* ppProc;
    PWSTR       pszLib;
    PSTR        pszProc;
    PWORD       pwOrd;
    UINT        iCch, iCchLib, iCchProc;
    UINT_PTR    uParamSize, uDelta;

    // Prepare parameters

    // uSize
    uParamSize = *(PUINT)lParam;
    uDelta = sizeof(UINT);
    if (uParamSize < uDelta)
        return STATUS_INVALID_PARAMETER;
    uParamSize -= uDelta;

    // szLibName
    pszLib = MOVE_PTR(lParam, uDelta, WCHAR);
    for (iCchLib = 0; uParamSize > 0; uParamSize--)
        if (!pszLib[iCchLib++])
            break;
    if (!uParamSize)
        return STATUS_INVALID_PARAMETER;

    // wProcOrdinal
    uDelta = sizeof(WORD);
    if (uParamSize < uDelta)
        return STATUS_INVALID_PARAMETER;
    uParamSize -= uDelta;
    pwOrd = (LPWORD)(pszLib + iCchLib);

    // szProcName and lpProc
    ppProc = (PVOID*)(pwOrd + 1);
    if (*pwOrd) {
        pszProc = MAKEINTRESOURCEA(*pwOrd);
        uDelta = sizeof(LPVOID);
        if (uParamSize < uDelta)
            return STATUS_INVALID_PARAMETER;
        iCchProc = 0;
    } else {
        // Alignment
        pszProc = (LPSTR)BYTE_ALIGN((ULONG_PTR)ppProc, STRING_ALIGNMENT);
        uDelta = (ULONG_PTR)pszProc - (ULONG_PTR)ppProc;
        if (uParamSize < uDelta)
            return STATUS_INVALID_PARAMETER;
        uParamSize -= uDelta;
        // szProcName
        if (pszProc[0] == '\0') {
            iCchProc = 0;
            pszProc = NULL;
            ppProc = (PVOID*)NULL;
        } else {
            for (iCchProc = 0; uParamSize > 0; uParamSize--)
                if (!pszProc[iCchProc++])
                    break;
            if (!uParamSize)
                return STATUS_INVALID_PARAMETER;
            ppProc = (PVOID*)(pszProc + iCchProc);
        }
    }

    // Start to load
    HMODULE hDLL = NULL, hNtDLL = Proc_GetNtdllHandle();
    // Get DLL handle if already loaded
    PLDR_DATA_TABLE_ENTRY pLdrNodeHead, pLdrNode;
    pLdrNodeHead = CONTAINING_RECORD(NT_GetPEB()->Ldr->InLoadOrderModuleList.Flink, LDR_DATA_TABLE_ENTRY, InLoadOrderModuleList);
    pLdrNode = pLdrNodeHead;
    do {
        if (pLdrNode->BaseDllName.MaximumLength == iCchLib * sizeof(WCHAR)) {
            for (iCch = 0; iCch < iCchLib; iCch++) {
                if (
                    pszLib[iCch] != pLdrNode->BaseDllName.Buffer[iCch] && (
                        (pszLib[iCch] >= 'a' && pszLib[iCch] <= 'z' && pszLib[iCch] - ('a' - 'A') != pLdrNode->BaseDllName.Buffer[iCch]) ||
                        (pszLib[iCch] >= 'A' && pszLib[iCch] <= 'Z' && pszLib[iCch] + ('a' - 'A') != pLdrNode->BaseDllName.Buffer[iCch])
                        )
                    )
                    break;
            }
            if (iCch == iCchLib) {
                hDLL = pLdrNode->DllBase;
                break;
            }
        }
        pLdrNode = CONTAINING_RECORD(pLdrNode->InLoadOrderModuleList.Flink, LDR_DATA_TABLE_ENTRY, InLoadOrderModuleList);
    } while (pLdrNode != pLdrNodeHead);
    if (hDLL && !pszProc)
        return STATUS_SUCCESS;

    // Get LdrLoadDll and LdrGetProcedureAddress address
    PFNLdrLoadDll               pLdrLoadDll = NULL;
    PFNLdrGetProcedureAddress   pLdrGetProcedureAddress = NULL;
    PVOID* ppLdrFunc;
    PIMAGE_DATA_DIRECTORY   pExportDir;
    PIMAGE_EXPORT_DIRECTORY pExportTable;
    PDWORD                  pdwFuncName;
    UINT                    uIndex;
    PDWORD                  padwNamesRVA;
    DWORD                   dwProcRVA;
    // Get export directory and table
    pExportDir = &MOVE_PTR(hNtDLL, ((PIMAGE_DOS_HEADER)hNtDLL)->e_lfanew, IMAGE_NT_HEADERS)->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT];
    pExportTable = MOVE_PTR(hNtDLL, pExportDir->VirtualAddress, IMAGE_EXPORT_DIRECTORY);
    // Search name
    padwNamesRVA = MOVE_PTR(hNtDLL, pExportTable->AddressOfNames, DWORD);
    for (uIndex = 0; uIndex < pExportTable->NumberOfNames; uIndex++, padwNamesRVA++) {
        // Match both of LdrLoadDll and LdrGetProcedureAddress
        if (*padwNamesRVA + ARRAYSIZE("LdrGetProcedureAddress") >= (DWORD_PTR)pExportDir->VirtualAddress + pExportDir->Size)
            return STATUS_INTERNAL_ERROR;
        pdwFuncName = MOVE_PTR(hNtDLL, *padwNamesRVA, DWORD);
        ppLdrFunc = NULL;
        // Fast comparison and avoid instructions like "xmmword ptr" generated
        if (!hDLL && !pLdrLoadDll &&
            pdwFuncName[0] == ('LrdL') &&
            pdwFuncName[1] == ('Ddao') &&
            *MOVE_PTR(pdwFuncName, 7, DWORD) == ('llD'))
            ppLdrFunc = (PVOID*)&pLdrLoadDll;
        else if (pszProc && !pLdrGetProcedureAddress &&
            pdwFuncName[0] == ('GrdL') &&
            pdwFuncName[1] == ('rPte') &&
            pdwFuncName[2] == ('deco') &&
            *MOVE_PTR(pdwFuncName, 11, DWORD) == ('erud') &&
            *MOVE_PTR(pdwFuncName, 15, DWORD) == ('rddA') &&
            *MOVE_PTR(pdwFuncName, 19, DWORD) == ('sse')
            )
            ppLdrFunc = (PVOID*)&pLdrGetProcedureAddress;
        if (ppLdrFunc) {
            // LdrLoadDll and LdrGetProcedureAddress are not forward thunks
            dwProcRVA = MOVE_PTR(hNtDLL, pExportTable->AddressOfFunctions, DWORD)[MOVE_PTR(hNtDLL, pExportTable->AddressOfNameOrdinals, WORD)[uIndex]];
            if (dwProcRVA < pExportDir->VirtualAddress || dwProcRVA >= pExportDir->VirtualAddress + pExportDir->Size)
                *ppLdrFunc = MOVE_PTR(hNtDLL, dwProcRVA, VOID);
            else
                return STATUS_INTERNAL_ERROR;
        }
        if ((hDLL || pLdrLoadDll) && (!pszProc || pLdrGetProcedureAddress)) {
            NTSTATUS lStatus = STATUS_SUCCESS;
            // Load DLL if not loaded yet
            if (!hDLL) {
                UNICODE_STRING  stLibName;
                ULONG           ulDllCharacteristics;
                stLibName.MaximumLength = (USHORT)(iCchLib * sizeof(WCHAR));
                stLibName.Length = stLibName.MaximumLength - sizeof(WCHAR);
                stLibName.Buffer = pszLib;
                lStatus = pLdrLoadDll(NULL, NULL, &stLibName, &hDLL);
                if (lStatus == STATUS_DLL_INIT_FAILED)
                    lStatus = pLdrLoadDll(NULL, (ulDllCharacteristics = IMAGE_FILE_EXECUTABLE_IMAGE, &ulDllCharacteristics), &stLibName, &hDLL);
                if (!NT_SUCCESS(lStatus))
                    return lStatus;
            }
            // Get procedure address
            if (pszProc) {
                PANSI_STRING    pstProcName;
                ULONG           ulProcOrd;
                if ((UINT_PTR)pszProc > MAXWORD) {
                    ANSI_STRING stProcName;
                    stProcName.MaximumLength = (USHORT)(iCchProc * sizeof(CHAR));
                    stProcName.Length = stProcName.MaximumLength - sizeof(CHAR);
                    stProcName.Buffer = pszProc;
                    pstProcName = &stProcName;
                    ulProcOrd = 0;
                } else {
                    pstProcName = NULL;
                    ulProcOrd = (ULONG)(ULONG_PTR)pszProc;
                }
                lStatus = pLdrGetProcedureAddress(hDLL, pstProcName, ulProcOrd, ppProc);
            }
            return lStatus;
        }
    }
    return STATUS_INTERNAL_ERROR;
}