#include "include\NTAssassin\NTAGDI.h"
#include "include\NTAssassin\NTAStr.h"
#include "include\NTAssassin\NTAUI.h"

BOOL NTAPI GDI_FillSolidRect(HDC DC, _In_ PRECT Rect, COLORREF Color) {
    COLORREF    crPrev = SetBkColor(DC, Color);
    BOOL        bRet = ExtTextOut(DC, 0, 0, ETO_OPAQUE, Rect, NULL, 0, NULL);
    SetBkColor(DC, crPrev);
    return bRet;
}

BOOL NTAPI GDI_FrameRect(HDC DC, PRECT Rect, INT Width, DWORD ROP) {
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

UINT NTAPI GDI_WriteBitmap(HDC DC, HBITMAP Bitmap, _Out_writes_bytes_opt_(BufferSize) PVOID Buffer, UINT BufferSize) {
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

static VOID NTAPI GDI_InitInternalFontInfo(_Out_ PENUMLOGFONTEXDVW FontInfo) {
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
    _In_opt_z_ PCWSTR Name) {
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

INT NTAPI GDI_GetFont(HFONT Font, PENUMLOGFONTEXDVW FontInfo) {
    GDI_InitInternalFontInfo(FontInfo);
    INT i = GetObjectW(Font, sizeof(FontInfo->elfEnumLogfontEx.elfLogFont), &FontInfo->elfEnumLogfontEx.elfLogFont);
    i = i > 0 ? sizeof(*FontInfo) : 0;
    return i;
}

BOOL NTAPI GDI_DrawIcon(HDC DC, _In_ HICON Icon, INT X, INT Y, INT CX, INT CY) {
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

BOOL NTAPI GDI_CreateSnapshot(HWND Window, _Out_ PGDI_SNAPSHOT Snapshot) {
    HDC     hDC;
    BOOL    bRet;
    if (Window) {
        RECT rc;
        GetClientRect(Window, &rc);
        Snapshot->Position.x = Snapshot->Position.y = 0;
        Snapshot->Size.cx = rc.right;
        Snapshot->Size.cy = rc.bottom;
    } else {
        UI_GetScreenPos(&Snapshot->Position, &Snapshot->Size);
    }
    hDC = GetDC(Window);
    Snapshot->DC = CreateCompatibleDC(hDC);
    Snapshot->Bitmap = CreateCompatibleBitmap(hDC, Snapshot->Size.cx, Snapshot->Size.cy);
    Snapshot->OriginalBitmap = SelectObject(Snapshot->DC, Snapshot->Bitmap);
    bRet = BitBlt(Snapshot->DC, 0, 0, Snapshot->Size.cx, Snapshot->Size.cy, hDC, Snapshot->Position.x, Snapshot->Position.y, SRCCOPY);
    ReleaseDC(Window, hDC);
    return bRet;
}

BOOL NTAPI GDI_DeleteSnapshot(_In_ PGDI_SNAPSHOT Snapshot) {
    BOOL bRet;
    bRet = DeleteDC(Snapshot->DC);
    bRet &= DeleteObject(Snapshot->Bitmap);
    bRet &= DeleteObject(Snapshot->OriginalBitmap);
    return bRet;
}
