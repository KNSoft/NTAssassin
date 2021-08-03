#include "include\NTAssassin\NTAssassin.h"

BOOL NTAPI PE_Init(PPE_IMAGE lpImage, LPVOID lpMem, BOOL bFileMap) {
    PIMAGE_NT_HEADERS   lpNtHeader;
    __try {
        if (((PIMAGE_DOS_HEADER)lpMem)->e_magic == IMAGE_DOS_SIGNATURE) {
            lpNtHeader = (PIMAGE_NT_HEADERS)((BYTE*)lpMem + ((PIMAGE_DOS_HEADER)lpMem)->e_lfanew);
            if (lpNtHeader->Signature == IMAGE_NT_SIGNATURE) {
                if ((lpNtHeader->FileHeader.Machine == IMAGE_FILE_MACHINE_I386 && lpNtHeader->OptionalHeader.Magic == IMAGE_NT_OPTIONAL_HDR32_MAGIC) || (lpNtHeader->FileHeader.Machine == IMAGE_FILE_MACHINE_AMD64 && lpNtHeader->OptionalHeader.Magic == IMAGE_NT_OPTIONAL_HDR64_MAGIC)) {
                    lpImage->lpImage = lpMem;
                    lpImage->lpNtHeader = lpNtHeader;
                    lpImage->wMachine = lpNtHeader->FileHeader.Machine;
                    return TRUE;
                }
            }
        }
        return FALSE;
    } __except (EXCEPTION_EXECUTE_HANDLER) {
        return FALSE;
    }
}