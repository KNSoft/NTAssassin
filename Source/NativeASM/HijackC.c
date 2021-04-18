#include "NTAssassin.h"

typedef NTSTATUS(NTAPI* PLdrLoadDll)(
    IN PWSTR SearchPath OPTIONAL,
    IN PULONG DllCharacteristics OPTIONAL,
    IN PUNICODE_STRING DllName,
    OUT PVOID* BaseAddress
    );


typedef NTSTATUS(NTAPI* PLdrGetProcedureAddress)(
    IN PVOID BaseAddress,
    IN PANSI_STRING Name,
    IN ULONG Ordinal,
    OUT PVOID* ProcedureAddress
    );

NTSTATUS WINAPI Hijack_LoadProcAddr_InjectThread(LPVOID lParam) {
    LPVOID*     lppProc;
    LPWSTR      lpszLib;
    LPSTR       lpszProc;
    LPWORD      lpwOrd;
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
    lpszLib = ADD_OFFSET(lParam, uDelta, WCHAR);
    for (iCchLib = 0; uParamSize > 0; uParamSize--)
        if (!lpszLib[iCchLib++])
            break;
    if (!uParamSize)
        return STATUS_INVALID_PARAMETER;

    // wProcOrdinal
    uDelta = sizeof(WORD);
    if (uParamSize < uDelta)
        return STATUS_INVALID_PARAMETER;
    uParamSize -= uDelta;
    lpwOrd = (LPWORD)(lpszLib + iCchLib);

    // szProcName and lpProc
    lppProc = (LPVOID*)(lpwOrd + 1);
    if (*lpwOrd) {
        lpszProc = MAKEINTRESOURCEA(*lpwOrd);
        uDelta = sizeof(LPVOID);
        if (uParamSize < uDelta)
            return STATUS_INVALID_PARAMETER;
        iCchProc = 0;
    } else {
        // Alignment
        lpszProc = (LPSTR)BYTE_ALIGN((ULONG_PTR)lppProc, STRING_ALIGNMENT);
        uDelta = (ULONG_PTR)lpszProc - (ULONG_PTR)lppProc;
        if (uParamSize < uDelta)
            return STATUS_INVALID_PARAMETER;
        uParamSize -= uDelta;
        // szProcName
        if (lpszProc[0] == '\0') {
            iCchProc = 0;
            lpszProc = NULL;
            lppProc = (LPVOID*)NULL;
        } else {
            for (iCchProc = 0; uParamSize > 0; uParamSize--)
                if (!lpszProc[iCchProc++])
                    break;
            if (!uParamSize)
                return STATUS_INVALID_PARAMETER;
            lppProc = (LPVOID*)(lpszProc + iCchProc);
        }
    }

    // Start to load
    HMODULE hDLL = NULL, hNtDLL = Proc_GetNtdllHandle();
    // Get DLL handle if already loaded
    PLDR_DATA_TABLE_ENTRY lpLdrNodeHead, lpLdrNode;
    lpLdrNodeHead = CONTAINING_RECORD(NT_GetPEB()->Ldr->InLoadOrderModuleList.Flink, LDR_DATA_TABLE_ENTRY, InLoadOrderModuleList);
    lpLdrNode = lpLdrNodeHead;
    do {
        if (lpLdrNode->BaseDllName.MaximumLength == iCchLib * sizeof(WCHAR)) {
            for (iCch = 0; iCch < iCchLib; iCch++) {
                if (
                    lpszLib[iCch] != lpLdrNode->BaseDllName.Buffer[iCch] && (
                        (lpszLib[iCch] >= 'a' && lpszLib[iCch] <= 'z' && lpszLib[iCch] - ('a' - 'A') != lpLdrNode->BaseDllName.Buffer[iCch]) ||
                        (lpszLib[iCch] >= 'A' && lpszLib[iCch] <= 'Z' && lpszLib[iCch] + ('a' - 'A') != lpLdrNode->BaseDllName.Buffer[iCch])
                        )
                    )
                    break;
            }
            if (iCch == iCchLib) {
                hDLL = lpLdrNode->DllBase;
                break;
            }
        }
        lpLdrNode = CONTAINING_RECORD(lpLdrNode->InLoadOrderModuleList.Flink, LDR_DATA_TABLE_ENTRY, InLoadOrderModuleList);
    } while (lpLdrNode != lpLdrNodeHead);
    if (hDLL && !lpszProc)
        return STATUS_SUCCESS;

    // Get LdrLoadDll and LdrGetProcedureAddress address
    PLdrLoadDll             lpLdrLoadDll = NULL;
    PLdrGetProcedureAddress lpLdrGetProcedureAddress = NULL;
    PVOID*                  lppLdrFunc;
    PIMAGE_DATA_DIRECTORY   lpExportDir;
    PIMAGE_EXPORT_DIRECTORY lpExportTable;
    PDWORD                  lpdwFuncName;
    UINT                    uIndex;
    PDWORD                  lpadwNamesRVA;
    DWORD                   dwProcRVA;
    // Get export directory and table
    lpExportDir = &ADD_OFFSET(hNtDLL, ((PIMAGE_DOS_HEADER)hNtDLL)->e_lfanew, IMAGE_NT_HEADERS)->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT];
    lpExportTable = ADD_OFFSET(hNtDLL, lpExportDir->VirtualAddress, IMAGE_EXPORT_DIRECTORY);
    // Search name
    lpadwNamesRVA = ADD_OFFSET(hNtDLL, lpExportTable->AddressOfNames, DWORD);
    for (uIndex = 0; uIndex < lpExportTable->NumberOfNames; uIndex++, lpadwNamesRVA++) {
        // Match both of LdrLoadDll and LdrGetProcedureAddress
        if (*lpadwNamesRVA + ARRAYSIZE("LdrGetProcedureAddress") >= (DWORD_PTR)lpExportDir->VirtualAddress + lpExportDir->Size)
            return STATUS_INTERNAL_ERROR;
        lpdwFuncName = ADD_OFFSET(hNtDLL, *lpadwNamesRVA, DWORD);
        lppLdrFunc = NULL;
        // Fast comparison and avoid instructions like "xmmword ptr" generated
        if (!hDLL && !lpLdrLoadDll &&
            lpdwFuncName[0] == ('LrdL') &&
            lpdwFuncName[1] == ('Ddao') &&
            *ADD_OFFSET(lpdwFuncName, 7, DWORD) == ('llD'))
            lppLdrFunc = (PVOID*)&lpLdrLoadDll;
        else if (lpszProc && !lpLdrGetProcedureAddress &&
            lpdwFuncName[0] == ('GrdL') &&
            lpdwFuncName[1] == ('rPte') &&
            lpdwFuncName[2] == ('deco') &&
            *ADD_OFFSET(lpdwFuncName, 11, DWORD) == ('erud') &&
            *ADD_OFFSET(lpdwFuncName, 15, DWORD) == ('rddA') &&
            *ADD_OFFSET(lpdwFuncName, 19, DWORD) == ('sse')
            )
            lppLdrFunc = (PVOID*)&lpLdrGetProcedureAddress;
        if (lppLdrFunc) {
            // LdrLoadDll and LdrGetProcedureAddress are not forward thunks
            dwProcRVA = ADD_OFFSET(hNtDLL, lpExportTable->AddressOfFunctions, DWORD)[ADD_OFFSET(hNtDLL, lpExportTable->AddressOfNameOrdinals, WORD)[uIndex]];
            if (dwProcRVA < lpExportDir->VirtualAddress || dwProcRVA >= lpExportDir->VirtualAddress + lpExportDir->Size)
                *lppLdrFunc = ADD_OFFSET(hNtDLL, dwProcRVA, VOID);
            else
                return STATUS_INTERNAL_ERROR;
        }
        if ((hDLL || lpLdrLoadDll) && (!lpszProc || lpLdrGetProcedureAddress)) {
            NTSTATUS lStatus = STATUS_SUCCESS;
            // Load DLL if not loaded yet
            if (!hDLL) {
                UNICODE_STRING  stLibName;
                ULONG           ulDllCharacteristics;
                stLibName.MaximumLength = (USHORT)(iCchLib * sizeof(WCHAR));
                stLibName.Length = stLibName.MaximumLength - sizeof(WCHAR);
                stLibName.Buffer = lpszLib;
                lStatus = lpLdrLoadDll(NULL, NULL, &stLibName, &hDLL);
                if (lStatus == STATUS_DLL_INIT_FAILED)
                    lStatus = lpLdrLoadDll(NULL, (ulDllCharacteristics = IMAGE_FILE_EXECUTABLE_IMAGE, &ulDllCharacteristics), &stLibName, &hDLL);
                if (!NT_SUCCESS(lStatus))
                    return lStatus;
            }
            // Get procedure address
            if (lpszProc) {
                PANSI_STRING    lpstProcName;
                ULONG           ulProcOrd;
                if ((UINT_PTR)lpszProc > MAXWORD) {
                    ANSI_STRING stProcName;
                    stProcName.MaximumLength = (USHORT)(iCchProc * sizeof(CHAR));
                    stProcName.Length = stProcName.MaximumLength - sizeof(CHAR);
                    stProcName.Buffer = lpszProc;
                    lpstProcName = &stProcName;
                    ulProcOrd = 0;
                } else {
                    lpstProcName = NULL;
                    ulProcOrd = (ULONG)(ULONG_PTR)lpszProc;
                }
                lStatus = lpLdrGetProcedureAddress(hDLL, lpstProcName, ulProcOrd, lppProc);
            }
            return lStatus;
        }
    }
    return STATUS_INTERNAL_ERROR;
}