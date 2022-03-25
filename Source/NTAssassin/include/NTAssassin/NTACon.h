#pragma once

#include "NTAssassin.h"

#define CON_INPUT_HANDLE (NT_GetPEB()->ProcessParameters->StandardInput)
#define CON_OUTPUT_HANDLE (NT_GetPEB()->ProcessParameters->StandardOutput)
#define CON_ERROR_HANDLE (NT_GetPEB()->ProcessParameters->StandardError)

NTA_API DWORD Con_Write(HANDLE ConHandle, PVOID Buffer, SIZE_T Length);

NTA_API DWORD Con_WriteString(HANDLE ConHandle, PCSTR String);

NTA_API DWORD Con_WriteLine(HANDLE ConHandle, PCSTR String);

NTA_API DWORD Con_VPrintf(HANDLE ConHandle, _In_z_ _Printf_format_string_ PCSTR Format, _In_ va_list ArgList);

NTA_API DWORD WINAPIV Con_Printf(HANDLE ConHandle, _In_z_ _Printf_format_string_ PCSTR Format, ...);

NTA_API DWORD WINAPIV Con_PrintfLine(HANDLE ConHandle, _In_z_ _Printf_format_string_ PCSTR Format, ...);