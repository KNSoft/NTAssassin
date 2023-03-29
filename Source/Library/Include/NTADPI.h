﻿// High-DPI support

#pragma once

#include "NTAssassin_Base.h"

/// <summary>
/// Gets DPI of window
/// </summary>
/// <param name="Window">Handle to the window</param>
/// <param name="DPIX">Pointer to an UINT variable to receive DPI-X</param>
/// <param name="DPIY">Pointer to an UINT variable to receive DPI-Y</param>
/// <returns>TRUE if DPI values returned by GetDpiForMonitor, or FALSE if returned by GetDeviceCaps</returns>
NTA_API BOOL NTAPI DPI_FromWindow(HWND Window, _Out_ PUINT DPIX, _Out_ PUINT DPIY);

/// <summary>
/// Scales a value according to given DPI
/// </summary>
/// <param name="Value">Pointer to the value to be scaled</param>
/// <param name="OldDPI">Old DPI value</param>
/// <param name="NewDPI">New DPI value</param>
NTA_API VOID NTAPI DPI_ScaleInt(_Inout_ PINT Value, _In_ UINT OldDPI, _In_ UINT NewDPI);
NTA_API VOID NTAPI DPI_ScaleUInt(_Inout_ PUINT Value, _In_ UINT OldDPI, _In_ UINT NewDPI);

/// <summary>
/// Scales a RECT according to given DPI
/// </summary>
/// <param name="Rect">Pointer to the RECT structure to be scaled</param>
/// <param name="OldDPIX">Old DPI-X value</param>
/// <param name="NewDPIX">New DPI-X value</param>
/// <param name="OldDPIY">Old DPI-Y value</param>
/// <param name="NewDPIY">New DPI-Y value</param>
VOID NTAPI DPI_ScaleRect(_Inout_ PRECT Rect, _In_ UINT OldDPIX, _In_ UINT NewDPIX, _In_ UINT OldDPIY, _In_ UINT NewDPIY);

/// <summary>
/// Subclasses a dialog box to support High-DPI
/// </summary>
/// <param name="Dialog">Handle to the dialog box</param>
/// <param name="FontInfo">Pointer to an ENUMLOGFONTEXDVW structure specifies font to use, NULL to use default UI font</param>
/// <returns>TRUE if subclass successfully, or FALSE if failed</returns>
NTA_API BOOL NTAPI DPI_SetAutoAdjustSubclass(_In_ HWND Dialog, _In_opt_ PENUMLOGFONTEXDVW FontInfo);

/// <summary>
/// Gets information about a dialog box subclassed by DPI_SetAutoAdjustSubclass
/// </summary>
/// <param name="Dialog">Handle to the dialog box</param>
/// <param name="NewDPIX">Current DPI-X</param>
/// <param name="NewDPIY">Current DPI-Y</param>
/// <param name="Font">Current font</param>
/// <returns>TRUE if subclass successfully, or FALSE if failed</returns>
NTA_API _Success_(return != FALSE) BOOL NTAPI DPI_GetAutoAdjustSubclass(_In_ HWND Dialog, _Out_opt_ PDWORD NewDPIX, _Out_opt_ PDWORD NewDPIY, _Out_opt_ HFONT * Font);
