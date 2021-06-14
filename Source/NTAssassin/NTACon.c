#include "NTAssassin\NTAssassin.h"

NTSTATUS NTAPI Con_Write(LPWSTR psz) {
	PEB *lpstPEB = NT_GetPEB();
	return STATUS_SUCCESS;
}
