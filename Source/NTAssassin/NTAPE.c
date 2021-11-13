#include "include\NTAssassin\NTAssassin.h"

BOOL NTAPI PE_Resolve(PPE_STRUCT PEStruct, PVOID Image, BOOL OfflineMap) {
    BOOL bRet = FALSE;
    __try {
        PIMAGE_DOS_HEADER   pDosHdr = (PIMAGE_DOS_HEADER)Image;
        if (pDosHdr->e_magic == IMAGE_DOS_SIGNATURE) {
            PDWORD              pNtSign = MOVE_PTR(pDosHdr, pDosHdr->e_lfanew, DWORD);
            PIMAGE_FILE_HEADER  pFileHdr;
            BOOL                bNTImage;
            if (*pNtSign == IMAGE_NT_SIGNATURE) {
                pFileHdr = MOVE_PTR(pNtSign, sizeof((*pNtSign)), IMAGE_FILE_HEADER);
                bNTImage = TRUE;
            } else {
                pFileHdr = (PIMAGE_FILE_HEADER)pNtSign;
                bNTImage = FALSE;
            }
            PIMAGE_OPTIONAL_HEADER  pOptHdr = MOVE_PTR(pFileHdr, sizeof((*pFileHdr)), VOID);
            if ((bNTImage && (pOptHdr->Magic == IMAGE_NT_OPTIONAL_HDR32_MAGIC || pOptHdr->Magic == IMAGE_NT_OPTIONAL_HDR64_MAGIC)) ||
                (!bNTImage && pOptHdr->Magic == IMAGE_ROM_OPTIONAL_HDR_MAGIC)) {
                PEStruct->Image = Image;
                PEStruct->OfflineMap = OfflineMap;
                PEStruct->FileHeader = pFileHdr;
                PEStruct->OptionalHeader = pOptHdr;
                PEStruct->SectionHeader = MOVE_PTR(pOptHdr, pFileHdr->SizeOfOptionalHeader, IMAGE_SECTION_HEADER);
                PEStruct->OverlayData = NULL;
                bRet = TRUE;
            }
        }
    } __except (EXCEPTION_EXECUTE_HANDLER) {
    }
    return bRet;
}

PIMAGE_DATA_DIRECTORY NTAPI PE_GetDataDirectory(PPE_STRUCT PEStruct, UINT Index) {
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

PIMAGE_SECTION_HEADER NTAPI PE_GetSectionByRVA(PPE_STRUCT PEStruct, DWORD RVA) {
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

PVOID NTAPI PE_RVA2Ptr(PPE_STRUCT PEStruct, DWORD RVA) {
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