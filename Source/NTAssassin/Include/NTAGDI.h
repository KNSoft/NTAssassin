﻿#pragma once

#include "NTAssassin_Base.h"

typedef struct _GDI_SNAPSHOT
{
    HDC     DC;
    HBITMAP Bitmap;
    POINT   Position;
    SIZE    Size;
} GDI_SNAPSHOT, *PGDI_SNAPSHOT;

/// <summary>
/// Fills a rectangle area with single color
/// </summary>
/// <seealso cref="CDC::FillSolidRect"/>
/// <returns>TRUE if succeeded, or FALSE if failed</returns>
NTA_API BOOL NTAPI GDI_FillSolidRect(HDC DC, _In_ PRECT Rect, COLORREF Color);

/// <summary>
/// Draws a rectangle frame with current brush and specified ROP
/// </summary>
/// <param name="DC">Handle to the DC</param>
/// <param name="Rect">Pointer to a RECT specifies rectangle area</param>
/// <param name="Width">Width of frame, positive number indicates outer frame, negative indicates inner frame</param>
/// <param name="ROP">ROP, see also <c>PatBlt</c></param>
/// <returns>TRUE if succeeded, or FALSE if failed</returns>
NTA_API BOOL NTAPI GDI_FrameRect(HDC DC, PRECT Rect, INT Width, DWORD ROP);

/// <summary>
/// Writes bitmap (DDB) data to buffer
/// <see href="https://docs.microsoft.com/en-US/windows/win32/gdi/storing-an-image">Storing an Image - MSDN</see>
/// <see href="https://docs.microsoft.com/en-us/windows/win32/gdi/bitmap-storage">Bitmap Storage - MSDN</see>
/// </summary>
/// <param name="DC">Handle to the DC</param>
/// <param name="Bitmap">Handle to the bitmap</param>
/// <param name="Buffer">Pointer to the buffer receives bitmap data, NULL if gets required size only</param>
/// <param name="BufferSize">Size of buffer, ignored if Buffer is NULL when acquire required size</param>
/// <returns>Size of data write in bytes, or size required if Buffer is NULL</returns>
/// <remarks>To writes a DIB, use "GDIP_LoadImageFromBitmap" and "GDIP_SaveImageToBMPFile" instead</remarks>
NTA_API UINT NTAPI GDI_WriteBitmap(HDC DC, HBITMAP Bitmap, _Out_writes_bytes_opt_(BufferSize) PVOID Buffer, UINT BufferSize);

/// <summary>
/// Initializes ENUMLOGFONTEXDVW structure
/// </summary>
/// <seealso cref="LOGFONT"/>
NTA_API VOID NTAPI GDI_InitFontInfoEx(
    _Out_ PENUMLOGFONTEXDVW FontInfo,
    LONG Height,
    LONG Width,
    LONG Escapement,
    LONG Orientation,
    LONG Weight,
    BOOL Italic,
    BOOL Underline,
    BOOL StrikeOut,
    BYTE CharSet,
    BYTE OutPrecision,
    BYTE ClipPrecision,
    BYTE Quality,
    BYTE PitchAndFamily,
    _In_opt_z_ PCWSTR Name);
#define GDI_InitFontInfo(FontInfo, Size, Weight, Name) GDI_InitFontInfoEx(FontInfo, Size, 0, 0, 0, Weight, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_DONTCARE, Name)

/// <summary>
/// Gets system default display font information
/// </summary>
/// <param name="FontInfo">Pointer to the ENUMLOGFONTEXDVW structure receives information</param>
/// <param name="Height">New height value overwrites outputs</param>
/// <returns>TRUE if succeeded, or FALSE if failed, error code storaged in last ERROR</returns>
NTA_API _Success_(return != FALSE) BOOL NTAPI GDI_GetDefaultFont(_Out_ PENUMLOGFONTEXDVW FontInfo, _In_opt_ LONG Height);

/// <summary>
/// Creates default UI font
/// </summary>
/// <returns>Handle to the font</returns>
NTA_API HFONT NTAPI GDI_CreateDefaultFont();

/// <summary>
/// Retrieves information for the font
/// </summary>
/// <seealso cref="GetObject"/>
/// <param name="Font">Handle to the font</param>
/// <param name="FontInfo">Pointer to the ENUMLOGFONTEXDVW structure receives information</param>
/// <returns>Number of bytes stored or 0 if failed</returns>
NTA_API _Success_(return > 0) INT NTAPI GDI_GetFont(_In_ HFONT Font, _Out_ PENUMLOGFONTEXDVW FontInfo);

/// <summary>
/// Draws an icon
/// </summary>
/// <seealso cref="DrawIcon"/>
NTA_API BOOL NTAPI GDI_DrawIcon(HDC DC, _In_ HICON Icon, INT X, INT Y, INT CX, INT CY);

/// <summary>
/// Creates snapshot for specified window
/// </summary>
/// <param name="Window">Handle to the window, or NULL to creates snapshot for screen</param>
/// <param name="Snapshot">Pointer to a GDI_SNAPSHOT structure receives the snapshot resources and information</param>
/// <returns>TRUE if succeeded, or FALSE if failed</returns>
/// <remarks>Snapshot should be deleted by <c>GDI_DeleteSnapshot</c> when no longer to be used</remarks>
_Success_(return != FALSE) NTA_API BOOL NTAPI GDI_CreateSnapshot(HWND Window, _Out_ PGDI_SNAPSHOT Snapshot);

/// <summary>
/// Deletes snapshot created by <c>GDI_CreateSnapshot</c>
/// </summary>
/// <param name="Snapshot">Pointer to a GDI_SNAPSHOT structure to delete</param>
NTA_API VOID NTAPI GDI_DeleteSnapshot(_In_ PGDI_SNAPSHOT Snapshot);
