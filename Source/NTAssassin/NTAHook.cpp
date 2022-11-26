#include "include\NTAssassin\NTADef.h"

EXTERN_C_START

#include "include\NTAssassin\NTAHook.h"

EXTERN_C_END

// Hook by using Detours
#include "NTAHook_Detours.cpp"

EXTERN_C_START

BOOL NTAPI Hook_Begin()
{
    return DetourTransactionBegin() == NO_ERROR;
}

BOOL NTAPI Hook_Set(BOOL Enable, PVOID* Address, PVOID HookAddress)
{
    return (Enable ? DetourAttach(Address, HookAddress) : DetourDetach(Address, HookAddress)) == NO_ERROR;
}

BOOL NTAPI Hook_Attach(PVOID* Address, PVOID HookAddress)
{
    return DetourAttach(Address, HookAddress) == NO_ERROR;
}

BOOL NTAPI Hook_Detach(PVOID* Address, PVOID HookAddress)
{
    return DetourDetach(Address, HookAddress) == NO_ERROR;
}

BOOL NTAPI Hook_Commit()
{
    return DetourTransactionCommit() == NO_ERROR;
}

EXTERN_C_END
