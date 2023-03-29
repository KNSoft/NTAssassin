#include "..\Include\Wintexports.h"

#include "..\Include\WIE_WinSta.h"
#include "..\Include\WIE_CommCtrl.h"

#pragma comment (lib, "ntdll.lib")


NTSTATUS NTAPI ExeMain()
{
    MessageBox(NULL, NULL, NULL, MB_OK);

    return STATUS_SUCCESS;
}

void __stdcall mainCRTStartup()
{
    NTSTATUS Status;
    Status = ExeMain();
    RtlExitUserProcess(Status);
}
