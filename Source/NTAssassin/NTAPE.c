#include "include\NTAssassin\NTAssassin.h"

_Success_(return != FALSE)
BOOL NTAPI PE_Resolve(_Out_ PPE_STRUCT PEStruct, _In_ PVOID Image, BOOL OfflineMap, SIZE_T OfflineMapFileSize) {
    BOOL bRet = FALSE;
    __try {
        PIMAGE_DOS_HEADER   pDosHdr = (PIMAGE_DOS_HEADER)Image;
        if (pDosHdr->e_magic == IMAGE_DOS_SIGNATURE) {
            PDWORD              pNtSign = MOVE_PTR(pDosHdr, pDosHdr->e_lfanew, DWORD);
            PIMAGE_FILE_HEADER  pFileHdr;
            if (*pNtSign == IMAGE_NT_SIGNATURE) {
                pFileHdr = MOVE_PTR(pNtSign, sizeof((*pNtSign)), IMAGE_FILE_HEADER);
                PIMAGE_OPTIONAL_HEADER pOptHdr = MOVE_PTR(pFileHdr, sizeof((*pFileHdr)), VOID);
                if (pOptHdr->Magic == IMAGE_NT_OPTIONAL_HDR32_MAGIC || pOptHdr->Magic == IMAGE_NT_OPTIONAL_HDR64_MAGIC) {
                    PIMAGE_SECTION_HEADER pSection = MOVE_PTR(pOptHdr, pFileHdr->SizeOfOptionalHeader, IMAGE_SECTION_HEADER);
                    PEStruct->Image = Image;
                    PEStruct->OfflineMap = OfflineMap;
                    PEStruct->FileHeader = pFileHdr;
                    PEStruct->OptionalHeader = pOptHdr;
                    PEStruct->SectionHeader = pSection;
                    if (OfflineMap && OfflineMapFileSize) {
                        USHORT u, uSections;
                        SIZE_T sFile = 0;
                        uSections = PEStruct->FileHeader->NumberOfSections;
                        pSection = PEStruct->SectionHeader;
                        for (u = 0; u < uSections; u++) {
                            SIZE_T sEndOfSec = (SIZE_T)pSection->PointerToRawData + pSection->SizeOfRawData;
                            if (sEndOfSec > sFile) {
                                sFile = sEndOfSec;
                            }
                            pSection++;
                        }
                        if (OfflineMapFileSize > sFile) {
                            PEStruct->OverlayData = MOVE_PTR(Image, sFile, VOID);
                            PEStruct->OverlayDataSize = OfflineMapFileSize - sFile;
                        }
                    } else {
                        PEStruct->OverlayData = NULL;
                        PEStruct->OverlayDataSize = 0;
                    }
                    bRet = TRUE;
                }
            }
        }
    } __except (EXCEPTION_EXECUTE_HANDLER) {
    }
    return bRet;
}

PIMAGE_DATA_DIRECTORY NTAPI PE_GetDataDirectory(_In_ PPE_STRUCT PEStruct, UINT Index) {
    PIMAGE_DATA_DIRECTORY pDataDirectory = NULL;
    __try {
        if (PEStruct->OptionalHeader->Magic == IMAGE_NT_OPTIONAL_HDR32_MAGIC)
            pDataDirectory = &PEStruct->OptionalHeader32->DataDirectory[Index];
        else if (PEStruct->OptionalHeader->Magic == IMAGE_NT_OPTIONAL_HDR64_MAGIC)
            pDataDirectory = &PEStruct->OptionalHeader64->DataDirectory[Index];
    } __except (EXCEPTION_EXECUTE_HANDLER) {
    }
    return pDataDirectory;
}

PIMAGE_SECTION_HEADER NTAPI PE_GetSectionByRVA(_In_ PPE_STRUCT PEStruct, DWORD RVA) {
    USHORT u, uSections;
    PIMAGE_SECTION_HEADER pTargetSection, pSection;
    pTargetSection = NULL;
    __try {
        uSections = PEStruct->FileHeader->NumberOfSections;
        pSection = PEStruct->SectionHeader;
        for (u = 0; u < uSections; u++) {
            DWORD dwSecVA = pSection->VirtualAddress;
            if (RVA >= dwSecVA && RVA < dwSecVA + pSection->Misc.VirtualSize) {
                pTargetSection = pSection;
                break;
            }
            pSection++;
        }
    } __except (EXCEPTION_EXECUTE_HANDLER) {
    }
    return pTargetSection;
}

PIMAGE_SECTION_HEADER NTAPI PE_GetSectionByOffset(_In_ PPE_STRUCT PEStruct, DWORD Offset) {
    USHORT u, uSections;
    PIMAGE_SECTION_HEADER pTargetSection, pSection;
    pTargetSection = NULL;
    __try {
        uSections = PEStruct->FileHeader->NumberOfSections;
        pSection = PEStruct->SectionHeader;
        for (u = 0; u < uSections; u++) {
            DWORD dwSecOffset = pSection->PointerToRawData;
            if (Offset >= dwSecOffset && Offset < dwSecOffset + pSection->SizeOfRawData) {
                pTargetSection = pSection;
                break;
            }
            pSection++;
        }
    } __except (EXCEPTION_EXECUTE_HANDLER) {
    }
    return pTargetSection;
}

PVOID NTAPI PE_RVA2Ptr(_In_ PPE_STRUCT PEStruct, DWORD RVA) {
    PVOID   Ptr = NULL;
    if (PEStruct->OfflineMap) {
        PIMAGE_SECTION_HEADER pSection = PE_GetSectionByRVA(PEStruct, RVA);
        if (pSection)
            Ptr = MOVE_PTR(PEStruct->Image, (ULONG_PTR)RVA - pSection->VirtualAddress + pSection->PointerToRawData, VOID);
    } else {
        Ptr = MOVE_PTR(PEStruct->Image, RVA, VOID);
    }
    return Ptr;
}

_Success_(return != FALSE)
BOOL NTAPI PE_Ptr2RVA(_In_ PPE_STRUCT PEStruct, _In_  PVOID Ptr, _Out_ PDWORD RVA) {
    DWORD dwDelta = (DWORD)((ULONG_PTR)Ptr - (ULONG_PTR)PEStruct->Image);
    if (PEStruct->OfflineMap) {
        PIMAGE_SECTION_HEADER pSection = PE_GetSectionByOffset(PEStruct, dwDelta);
        if (pSection) {
            *RVA = dwDelta - pSection->PointerToRawData + pSection->VirtualAddress;
        } else {
            return FALSE;
        }
    } else {
        *RVA = dwDelta;
    }
    return TRUE;
}

_Success_(return != FALSE)
BOOL NTAPI PE_Ptr2Offset(_In_ PPE_STRUCT PEStruct, _In_ PVOID Ptr, _Out_ PDWORD Offset) {
    DWORD dwDelta = (DWORD)((ULONG_PTR)Ptr - (ULONG_PTR)PEStruct->Image);
    if (PEStruct->OfflineMap) {
        *Offset = dwDelta;
    } else {
        PIMAGE_SECTION_HEADER pSection = PE_GetSectionByRVA(PEStruct, dwDelta);
        if (pSection) {
            *Offset = dwDelta - pSection->VirtualAddress + pSection->PointerToRawData;
        } else {
            return FALSE;
        }
    }
    return TRUE;
}

_Success_(return != FALSE)
BOOL NTAPI PE_GetExportedName(_In_ PPE_STRUCT PEStruct, _In_ PVOID Function, _Out_ PZPCSTR Name) {
    BOOL bRet = FALSE;
    DWORD dwFuncRVA;
    if (PE_Ptr2RVA(PEStruct, Function, &dwFuncRVA)) {
        __try {
            PIMAGE_DATA_DIRECTORY pExportDir = PE_GetDataDirectory(PEStruct, IMAGE_DIRECTORY_ENTRY_EXPORT);
            if (pExportDir) {
                PIMAGE_EXPORT_DIRECTORY pExportTable = PE_RVA2Ptr(PEStruct, pExportDir->VirtualAddress);
                if (pExportTable) {
                    UINT uFuncIndex;
                    PDWORD padwFuncRVAs = PE_RVA2Ptr(PEStruct, pExportTable->AddressOfFunctions);
                    for (uFuncIndex = 0; uFuncIndex < pExportTable->NumberOfFunctions; uFuncIndex++, padwFuncRVAs++) {
                        if (*padwFuncRVAs == dwFuncRVA) {
                            break;
                        }
                    }
                    if (uFuncIndex < pExportTable->NumberOfFunctions) {
                        PCSTR pszName = NULL;
                        UINT uNameIndex;
                        PWORD pawNameOrds = PE_RVA2Ptr(PEStruct, pExportTable->AddressOfNameOrdinals);
                        for (uNameIndex = 0; uNameIndex < pExportTable->NumberOfNames; uNameIndex++, pawNameOrds++) {
                            if (*pawNameOrds == (WORD)uFuncIndex) {
                                break;
                            }
                        }
                        if (uNameIndex < pExportTable->NumberOfNames) {
                            PDWORD padwNameRVAs = PE_RVA2Ptr(PEStruct, pExportTable->AddressOfNames);
                            if (padwNameRVAs) {
                                pszName = PE_RVA2Ptr(PEStruct, padwNameRVAs[uNameIndex]);
                            }
                        }
                        *Name = IF_NULL(pszName, (PCSTR)(DWORD_PTR)(uFuncIndex + (DWORD_PTR)pExportTable->Base));
                        bRet = TRUE;
                    }
                }
            }
        } __except (EXCEPTION_EXECUTE_HANDLER) {
        }
    }
    return bRet;
}