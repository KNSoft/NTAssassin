#include "include\NTAssassin\NTAssassin.h"

// Hook by using Detours
#include "3rdParty\src\Detours\detours.cpp"
#include "3rdParty\src\Detours\disasm.cpp"
#include "3rdParty\src\Detours\modules.cpp"

BOOL NTAPI Hook_Begin() {
    return DetourTransactionBegin() == NO_ERROR;
}

BOOL NTAPI Hook_Attach(PVOID* Address, PVOID HookAddress) {
    return DetourAttach(Address, HookAddress) == NO_ERROR;
}

BOOL NTAPI Hook_Detach(PVOID* Address, PVOID HookAddress) {
    return DetourDetach(Address, HookAddress) == NO_ERROR;
}

BOOL NTAPI Hook_Commit() {
    return DetourTransactionCommit() == NO_ERROR;
}