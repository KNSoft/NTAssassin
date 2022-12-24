#include "include\NTAssassin\NTAGDI.h"

#include <WindowsX.h>

#include "include\NTAssassin\NTAStr.h"
#include "include\NTAssassin\NTAUI.h"

BOOL NTAPI GDI_FillSolidRect(HDC DC, _In_ PRECT Rect, COLORREF Color)
{
    COLORREF crPrev = SetBkColor(DC, Color);
    BOOL bRet = ExtTextOut(DC, 0, 0, ETO_OPAQUE, Rect, NULL, 0, NULL);
    SetBkColor(DC, crPrev);
    return bRet;
}

BOOL NTAPI GDI_FrameRect(HDC DC, PRECT Rect, INT Width, DWORD ROP)
{
    return Width >= 0
        ?
        PatBlt(DC, Rect->left - Width, Rect->top - Width, Width, Rect->bottom - Rect->top + Width * 2, ROP) &&
        PatBlt(DC, Rect->right, Rect->top - Width, Width, Rect->bottom - Rect->top + Width * 2, ROP) &&
        PatBlt(DC, Rect->left, Rect->top - Width, Rect->right - Rect->left, Width, ROP) &&
        PatBlt(DC, Rect->left, Rect->bottom, Rect->right - Rect->left, Width, ROP)
        :
        PatBlt(DC, Rect->left, Rect->top, -Width, Rect->bottom - Rect->top, ROP) &&
        PatBlt(DC, Rect->right + Width, Rect->top, -Width, Rect->bottom - Rect->top, ROP) &&
        PatBlt(DC, Rect->left - Width, Rect->top, Rect->right - Rect->left + Width * 2, -Width, ROP) &&
        PatBlt(DC, Rect->left - Width, Rect->bottom + Width, Rect->right - Rect->left + Width * 2, -Width, ROP);
}

UINT NTAPI GDI_WriteBitmap(HDC DC, HBITMAP Bitmap, _Out_writes_bytes_opt_(BufferSize) PVOID Buffer, UINT BufferSize)
{
    BITMAP              bmp;
    ULONG               cClrBits;
    PBITMAPFILEHEADER   pbmfh;
    PBITMAPINFO         pbmi;
    DWORD               dwClrItem, dwClrSize;
    UINT                uHeadersSize, uFileSize, uImageSize;
    PVOID               pBits;
    // Get BITMAP structure
    if (!GetObject(Bitmap, sizeof(BITMAP), &bmp))
        return STATUS_INVALID_PARAMETER;
    // Calculate count of bits and color table items
    cClrBits = bmp.bmPlanes * bmp.bmBitsPixel;
    if (cClrBits == 1)
        cClrBits = 1;
    else if (cClrBits <= 4)
        cClrBits = 4;
    else if (cClrBits <= 8)
        cClrBits = 8;
    else if (cClrBits <= 16)
        cClrBits = 16;
    else if (cClrBits <= 24)
        cClrBits = 24;
    else
        cClrBits = 32;
    if (cClrBits < 24) {
        dwClrItem = 1 << cClrBits;
        dwClrSize = dwClrItem * sizeof(RGBQUAD);
    } else
        dwClrItem = dwClrSize = 0;
    // Calculate size of image and file
    uHeadersSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + dwClrSize;
    uImageSize = BYTE_ALIGN(bmp.bmWidth * cClrBits, 32) / 8 * bmp.bmHeight;
    uFileSize = uHeadersSize + uImageSize;
    // Write bitmap
    if (Buffer && BufferSize >= uFileSize) {
        pbmfh = (PBITMAPFILEHEADER)Buffer;
        pbmfh->bfType = 'MB';
        pbmfh->bfOffBits = uHeadersSize;
        pbmfh->bfSize = uFileSize;
        pbmfh->bfReserved1 = pbmfh->bfReserved2 = 0;
        pbmi = MOVE_PTR(pbmfh, sizeof(*pbmfh), BITMAPINFO);
        pbmi->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
        pbmi->bmiHeader.biWidth = bmp.bmWidth;
        pbmi->bmiHeader.biHeight = bmp.bmHeight;
        pbmi->bmiHeader.biPlanes = bmp.bmPlanes;
        pbmi->bmiHeader.biBitCount = bmp.bmBitsPixel;
        pbmi->bmiHeader.biSizeImage = uImageSize;
        pbmi->bmiHeader.biClrUsed = dwClrItem;
        pbmi->bmiHeader.biCompression = BI_RGB;
        pbmi->bmiHeader.biXPelsPerMeter = pbmi->bmiHeader.biYPelsPerMeter = pbmi->bmiHeader.biClrImportant = 0;
        pBits = MOVE_PTR(pbmi, pbmi->bmiHeader.biSize + (DWORD_PTR)dwClrSize, VOID);
        if (!GetDIBits(DC, Bitmap, 0, bmp.bmHeight, pBits, pbmi, DIB_RGB_COLORS)) {
            return 0;
        }
    }
    return uFileSize;
}

static VOID NTAPI GDI_InitInternalFontInfo(_Out_ PENUMLOGFONTEXDVW FontInfo)
{
    FontInfo->elfEnumLogfontEx.elfFullName[0] = FontInfo->elfEnumLogfontEx.elfStyle[0] = FontInfo->elfEnumLogfontEx.elfScript[0] = '\0';
    FontInfo->elfDesignVector.dvReserved = STAMP_DESIGNVECTOR;
    FontInfo->elfDesignVector.dvNumAxes = 0;
}

VOID NTAPI GDI_InitFontInfoEx(
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
    _In_opt_z_ PCWSTR Name)
{
    GDI_InitInternalFontInfo(FontInfo);
    PLOGFONTW pFontInfo = &FontInfo->elfEnumLogfontEx.elfLogFont;
    pFontInfo->lfHeight = Height;
    pFontInfo->lfWidth = Width;
    pFontInfo->lfEscapement = Escapement;
    pFontInfo->lfOrientation = Orientation;
    pFontInfo->lfWeight = Weight;
    pFontInfo->lfItalic = Italic;
    pFontInfo->lfUnderline = Underline;
    pFontInfo->lfStrikeOut = StrikeOut;
    pFontInfo->lfCharSet = CharSet;
    pFontInfo->lfOutPrecision = OutPrecision;
    pFontInfo->lfClipPrecision = ClipPrecision;
    pFontInfo->lfQuality = Quality;
    pFontInfo->lfPitchAndFamily = PitchAndFamily;
    if (Name == NULL) {
        pFontInfo->lfFaceName[0] = '\0';
    } else {
        Str_CopyW(pFontInfo->lfFaceName, Name);
    }
}

_Success_(return != FALSE)
BOOL NTAPI GDI_GetDefaultFont(_Out_ PENUMLOGFONTEXDVW FontInfo, _In_opt_ LONG Height)
{
    BOOL bRet;
    NONCLIENTMETRICSW ncm;
    ncm.cbSize = sizeof(ncm);
    bRet = SystemParametersInfoW(SPI_GETNONCLIENTMETRICS, sizeof(ncm), &ncm, 0);
    if (bRet) {
        GDI_InitInternalFontInfo(FontInfo);
        RtlCopyMemory(&FontInfo->elfEnumLogfontEx.elfLogFont, &ncm.lfMessageFont, sizeof(FontInfo->elfEnumLogfontEx.elfLogFont));
        if (Height) {
            FontInfo->elfEnumLogfontEx.elfLogFont.lfHeight = Height;
            FontInfo->elfEnumLogfontEx.elfLogFont.lfWidth = 0;
        }
    }
    return bRet;
}

HFONT NTAPI GDI_CreateDefaultFont()
{
    HFONT hFont = NULL;
    ENUMLOGFONTEXDVW FontInfo;
    if (GDI_GetDefaultFont(&FontInfo, 0)) {
        hFont = CreateFontIndirectExW(&FontInfo);
    }
    return hFont;
}

_Success_(return > 0)
INT NTAPI GDI_GetFont(_In_ HFONT Font, _Out_ PENUMLOGFONTEXDVW FontInfo)
{
    GDI_InitInternalFontInfo(FontInfo);
    INT i = GetObjectW(Font, sizeof(FontInfo->elfEnumLogfontEx.elfLogFont), &FontInfo->elfEnumLogfontEx.elfLogFont);
    i = i > 0 ? sizeof(*FontInfo) : 0;
    return i;
}

BOOL NTAPI GDI_DrawIcon(HDC DC, _In_ HICON Icon, INT X, INT Y, INT CX, INT CY)
{
    ICONINFO    stIconInfo;
    BITMAP      stBmp;
    RECT        rcDst;
    INT         iCX, iCY, iIconHeight;
    BOOL        bRet = FALSE;

    // Get icon info and bitmaps
    if (!GetIconInfo(Icon, &stIconInfo))
        return FALSE;
    if (GetObject(stIconInfo.hbmMask, sizeof(stBmp), &stBmp)) {
        // Height of icon is half of bmHeight member if has mask bitmap only
        iIconHeight = stIconInfo.hbmColor ? stBmp.bmHeight : (stBmp.bmHeight / 2);
        if (CX && CY) {
            iCX = CX;
            iCY = CY;
        } else {
            iCX = stBmp.bmWidth;
            iCY = iIconHeight;
        }
        // Prepare memory DC
        HDC     hDC;
        hDC = CreateCompatibleDC(DC);
        SelectBitmap(hDC, stIconInfo.hbmMask);
        rcDst.left = X;
        rcDst.top = Y;
        rcDst.right = X + iCX;
        rcDst.bottom = Y + iCY;
        GDI_FillSolidRect(DC, &rcDst, RGB(255, 255, 255));
        // Draw mask bitmap
        if (StretchBlt(DC, X, Y, iCX, iCY, hDC, 0, 0, stBmp.bmWidth, iIconHeight, SRCAND))
            // Draw color bitmap or bottom half of mask bitmap
            if (stIconInfo.hbmColor) {
                SelectBitmap(hDC, stIconInfo.hbmColor);
                if (StretchBlt(DC, X, Y, iCX, iCY, hDC, 0, 0, stBmp.bmWidth, iIconHeight, SRCAND))
                    bRet = TRUE;
            } else {
                if (StretchBlt(DC, X, Y, iCX, iCY, hDC, 0, iIconHeight, stBmp.bmWidth, iIconHeight, SRCINVERT))
                    bRet = TRUE;
            }
        DeleteDC(hDC);
    }
    // Cleanup
    DeleteObject(stIconInfo.hbmMask);
    if (stIconInfo.hbmColor)
        DeleteObject(stIconInfo.hbmColor);
    return bRet;
}

BOOL NTAPI GDI_CreateSnapshot(HWND Window, _Out_ PGDI_SNAPSHOT Snapshot)
{
    POINT   pt;
    SIZE    size;
    HDC     hDC, hMemDC;
    HBITMAP hBmp;
    if (Window) {
        RECT rc;
        GetClientRect(Window, &rc);
        pt.x = pt.y = 0;
        size.cx = rc.right;
        size.cy = rc.bottom;
    } else {
        UI_GetScreenPos(&pt, &size);
    }
    hDC = GetDC(Window);
    if (!hDC) {
        goto _Error_0;
    }
    hMemDC = CreateCompatibleDC(hDC);
    if (!hMemDC) {
        goto _Error_1;
    }
    hBmp = CreateCompatibleBitmap(hDC, size.cx, size.cy);
    if (!hBmp) {
        goto _Error_2;
    }
    SelectObject(hMemDC, hBmp);
    if (BitBlt(hMemDC, 0, 0, size.cx, size.cy, hDC, pt.x, pt.y, SRCCOPY)) {
        Snapshot->DC = hMemDC;
        Snapshot->Bitmap = hBmp;
        Snapshot->Position = pt;
        Snapshot->Size = size;
        return TRUE;
    }

_Error_2:
    DeleteDC(hMemDC);
_Error_1:
    ReleaseDC(Window, hDC);
_Error_0:
    return FALSE;
}

VOID NTAPI GDI_DeleteSnapshot(_In_ PGDI_SNAPSHOT Snapshot)
{
    DeleteDC(Snapshot->DC);
    DeleteObject(Snapshot->Bitmap);
}
