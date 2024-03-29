﻿#include "Include\NTAPE.h"

VOID NTAPI PE_ResolveOnline(_Out_ PPE_STRUCT PEStruct, _In_ HMODULE Image)
{
    PIMAGE_DOS_HEADER pDosHeader = (PIMAGE_DOS_HEADER)Image;
    PIMAGE_NT_HEADERS pNtHeader = Add2Ptr(pDosHeader, pDosHeader->e_lfanew);
    PIMAGE_FILE_HEADER pFileHeader = &pNtHeader->FileHeader;
    PIMAGE_OPTIONAL_HEADER pOptHeader = &pNtHeader->OptionalHeader;
    PIMAGE_SECTION_HEADER pSectionHeader = Add2Ptr(pOptHeader, pFileHeader->SizeOfOptionalHeader);
    PEStruct->Image = (PBYTE)Image;
    PEStruct->ImageSize = pOptHeader->SizeOfImage;
    PEStruct->FileHeader = pFileHeader;
    PEStruct->OptionalHeader = pOptHeader;
    PEStruct->SectionHeader = pSectionHeader;
    PEStruct->OfflineMap = FALSE;
    PEStruct->OverlayData = NULL;
    PEStruct->OverlayDataSize = 0;
}

_Success_(return != FALSE)
BOOL NTAPI PE_ResolveOffline(_Out_ PPE_STRUCT PEStruct, _In_reads_bytes_(BufferSize) PVOID Buffer, _In_ ULONG BufferSize)
{
    /* Boundary check */
    PBYTE pEndOfMap = Add2Ptr(Buffer, BufferSize);

    /* DOS Header */
    PIMAGE_DOS_HEADER pDosHeader = (PIMAGE_DOS_HEADER)Buffer;
    if (BufferSize < sizeof(IMAGE_DOS_HEADER) ||
        pDosHeader->e_magic != IMAGE_DOS_SIGNATURE ||
        pDosHeader->e_lfanew < sizeof(IMAGE_DOS_HEADER) ||
        BufferSize < pDosHeader->e_lfanew + RTL_SIZEOF_THROUGH_FIELD(IMAGE_NT_HEADERS, FileHeader)) {
        return FALSE;
    }

    /* NT Headers */
    PIMAGE_NT_HEADERS pNtHeader = Add2Ptr(pDosHeader, pDosHeader->e_lfanew);
    if (pNtHeader->Signature != IMAGE_NT_SIGNATURE) {
        return FALSE;
    }
    PIMAGE_FILE_HEADER pFileHeader = &pNtHeader->FileHeader;
    PIMAGE_OPTIONAL_HEADER pOptHeader = &pNtHeader->OptionalHeader;

    /* Section Headers */
    PIMAGE_SECTION_HEADER pSecHeader = Add2Ptr(pOptHeader, pFileHeader->SizeOfOptionalHeader);
    PBYTE pSecHeaderEnd = Add2Ptr(pSecHeader, sizeof(IMAGE_SECTION_HEADER) * ((SIZE_T)pFileHeader->NumberOfSections + 1));
    if (pSecHeaderEnd > pEndOfMap ||
        (pOptHeader->Magic != IMAGE_NT_OPTIONAL_HDR32_MAGIC && pOptHeader->Magic != IMAGE_NT_OPTIONAL_HDR64_MAGIC)) {
        return FALSE;
    }

    /* Overlay */
    USHORT u;
    ULONG ulFileSize = 0;
    PIMAGE_SECTION_HEADER pSection = pSecHeader;
    for (u = 0; u < pFileHeader->NumberOfSections; u++) {
        ULONG ulEndOfSec = pSection->PointerToRawData + pSection->SizeOfRawData;
        if (ulEndOfSec > BufferSize) {
            return FALSE;
        } else if (ulEndOfSec > ulFileSize) {
            ulFileSize = ulEndOfSec;
        }
        pSection++;
    }

    /* Fill Structure */
    if (BufferSize > ulFileSize) {
        PEStruct->OverlayData = Add2Ptr(Buffer, ulFileSize);
        PEStruct->OverlayDataSize = BufferSize - ulFileSize;
    } else {
        PEStruct->OverlayData = NULL;
        PEStruct->OverlayDataSize = 0;
    }
    PEStruct->Image = Buffer;
    PEStruct->ImageSize = BufferSize;
    PEStruct->OfflineMap = TRUE;
    PEStruct->FileHeader = pFileHeader;
    PEStruct->OptionalHeader = pOptHeader;
    PEStruct->SectionHeader = pSecHeader;

    return TRUE;
}

UINT NTAPI PE_GetBits(_In_ PPE_STRUCT PEStruct)
{
    UINT uBits = 0;
    if (PEStruct->OptionalHeader->Magic == IMAGE_NT_OPTIONAL_HDR32_MAGIC) {
        uBits = 32;
    } else if (PEStruct->OptionalHeader->Magic == IMAGE_NT_OPTIONAL_HDR64_MAGIC) {
        uBits = 64;
    }
    return uBits;
}

ULONGLONG NTAPI PE_GetOptionalHeaderValueEx(_In_ PPE_STRUCT PEStruct, ULONG FieldOffset, ULONG FieldSize)
{
    ULONGLONG ull = 0;
    PBYTE p = (PBYTE)PEStruct->OptionalHeader;
    if (FieldSize == sizeof(BYTE)) {
        ull = *(PBYTE)(p + FieldOffset);
    } else if (FieldSize == sizeof(USHORT)) {
        ull = *(PUSHORT)(p + FieldOffset);
    } else if (FieldSize == sizeof(ULONG)) {
        ull = *(PULONG)(p + FieldOffset);
    } else if (FieldSize == sizeof(ULONGLONG)) {
        ull = *(PULONGLONG)(p + FieldOffset);
    }
    return ull;
}

PIMAGE_DATA_DIRECTORY NTAPI PE_GetDataDirectory(_In_ PPE_STRUCT PEStruct, UINT Index)
{
    PIMAGE_DATA_DIRECTORY pDataDirectory = NULL;
    if (PEStruct->OptionalHeader->Magic == IMAGE_NT_OPTIONAL_HDR32_MAGIC) {
        pDataDirectory = &PEStruct->OptionalHeader32->DataDirectory[Index];
    } else if (PEStruct->OptionalHeader->Magic == IMAGE_NT_OPTIONAL_HDR64_MAGIC) {
        pDataDirectory = &PEStruct->OptionalHeader64->DataDirectory[Index];
    }
    return pDataDirectory;
}

PIMAGE_SECTION_HEADER NTAPI PE_GetSectionByRVA(_In_ PPE_STRUCT PEStruct, DWORD RVA)
{
    USHORT u, uSections;
    PIMAGE_SECTION_HEADER pTargetSection, pSection;
    pTargetSection = NULL;
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
    return pTargetSection;
}

PIMAGE_SECTION_HEADER NTAPI PE_GetSectionByOffset(_In_ PPE_STRUCT PEStruct, DWORD Offset)
{
    USHORT u, uSections;
    PIMAGE_SECTION_HEADER pTargetSection, pSection;
    pTargetSection = NULL;
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
    return pTargetSection;
}

PVOID NTAPI PE_RVA2Ptr(_In_ PPE_STRUCT PEStruct, DWORD RVA)
{
    ULONG uDelta;
    if (PEStruct->OfflineMap) {
        PIMAGE_SECTION_HEADER pSection = PE_GetSectionByRVA(PEStruct, RVA);
        if (pSection) {
            uDelta = RVA - pSection->VirtualAddress + pSection->PointerToRawData;
        } else {
            return NULL;
        }
    } else {
        uDelta = RVA;
    }
    return uDelta < PEStruct->ImageSize ? Add2Ptr(PEStruct->Image, uDelta) : NULL;
}

_Success_(return != FALSE)
BOOL NTAPI PE_Ptr2RVA(_In_ PPE_STRUCT PEStruct, _In_ PVOID Ptr, _Out_ PDWORD RVA)
{
    ULONG uDelta = (ULONG)((ULONG_PTR)Ptr - (ULONG_PTR)PEStruct->Image);
    if (PEStruct->OfflineMap) {
        PIMAGE_SECTION_HEADER pSection = PE_GetSectionByOffset(PEStruct, uDelta);
        if (pSection) {
            uDelta = uDelta - pSection->PointerToRawData + pSection->VirtualAddress;
        } else {
            return FALSE;
        }
    }
    *RVA = uDelta;
    return TRUE;
}

_Success_(return != FALSE)
BOOL NTAPI PE_Ptr2Offset(_In_ PPE_STRUCT PEStruct, _In_ PVOID Ptr, _Out_ PDWORD Offset)
{
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
BOOL NTAPI PE_GetExportedName(_In_ PPE_STRUCT PEStruct, _In_ PVOID Function, _Out_ PZPCSTR Name)
{
    /* Get address of export table */
    DWORD dwFuncRVA;
    if (!PE_Ptr2RVA(PEStruct, Function, &dwFuncRVA)) {
        return FALSE;
    }
    PIMAGE_DATA_DIRECTORY pExportDir = PE_GetDataDirectory(PEStruct, IMAGE_DIRECTORY_ENTRY_EXPORT);
    if (!pExportDir ||
        !PE_ProbeForRead(PEStruct, pExportDir, sizeof(IMAGE_DATA_DIRECTORY))) {
        return FALSE;
    }
    PIMAGE_EXPORT_DIRECTORY pExportTable = PE_RVA2Ptr(PEStruct, pExportDir->VirtualAddress);
    if (!pExportTable ||
        !PE_ProbeForRead(PEStruct, pExportTable, sizeof(IMAGE_EXPORT_DIRECTORY))) {
        return FALSE;
    }
    PDWORD padwFuncRVAs = PE_RVA2Ptr(PEStruct, pExportTable->AddressOfFunctions);
    if (!padwFuncRVAs ||
        !PE_ProbeForRead(PEStruct, padwFuncRVAs, pExportTable->NumberOfFunctions * sizeof(*padwFuncRVAs))) {
        return FALSE;
    }

    /* Find exported functions */
    UINT uFuncIndex;
    for (uFuncIndex = 0; uFuncIndex < pExportTable->NumberOfFunctions; uFuncIndex++, padwFuncRVAs++) {
        if (*padwFuncRVAs == dwFuncRVA) {
            break;
        }
    }
    if (uFuncIndex >= pExportTable->NumberOfFunctions) {
        return FALSE;
    }

    /* Get name of function */
    PCSTR pszName = NULL;
    PWORD pawNameOrds = PE_RVA2Ptr(PEStruct, pExportTable->AddressOfNameOrdinals);
    if (!pawNameOrds ||
        !PE_ProbeForRead(PEStruct, pawNameOrds, pExportTable->NumberOfNames * sizeof(*pawNameOrds))) {
        return FALSE;
    }
    UINT uNameIndex;
    for (uNameIndex = 0; uNameIndex < pExportTable->NumberOfNames; uNameIndex++, pawNameOrds++) {
        if (*pawNameOrds == (WORD)uFuncIndex) {
            break;
        }
    }
    if (uNameIndex >= pExportTable->NumberOfNames) {
        return FALSE;
    }
    PDWORD padwNameRVAs = PE_RVA2Ptr(PEStruct, pExportTable->AddressOfNames);
    if (padwNameRVAs &&
        PE_ProbeForRead(PEStruct, &padwNameRVAs[uNameIndex], sizeof(*padwNameRVAs))) {
        pszName = PE_RVA2Ptr(PEStruct, padwNameRVAs[uNameIndex]);
    }
    *Name = pszName ? pszName : (PCSTR)(DWORD_PTR)(uFuncIndex + (DWORD_PTR)pExportTable->Base);
    return TRUE;
}

LPWIN_CERTIFICATE PE_GetCertificate(_In_ PPE_STRUCT PEStruct)
{
    LPWIN_CERTIFICATE pCertificate;
    PIMAGE_DATA_DIRECTORY pCertificateDir;

    if (PEStruct->OfflineMap == FALSE)
    {
        return NULL;
    }

    pCertificateDir = PE_GetDataDirectory(PEStruct, IMAGE_DIRECTORY_ENTRY_SECURITY);
    if (pCertificateDir == NULL ||
        !PE_ProbeForRead(PEStruct, pCertificateDir, sizeof(*pCertificateDir)) ||
        pCertificateDir->VirtualAddress == 0 ||
        pCertificateDir->Size == 0)
    {
        return NULL;
    }

    pCertificate = Add2Ptr(PEStruct->Image, pCertificateDir->VirtualAddress);
    if (!PE_ProbeForRead(PEStruct, pCertificate, pCertificateDir->Size))
    {
        return NULL;
    }

    return pCertificate;
}
