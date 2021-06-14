#include "NTAssassin\NTAssassin.h"

BOOL NTAPI GDI_FillSolidRect(HDC DC, PRECT Rect, COLORREF Color) {
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

// https://docs.microsoft.com/en-US/windows/win32/gdi/storing-an-image
// https://docs.microsoft.com/en-us/windows/win32/api/wingdi/ns-wingdi-bitmap
// https://docs.microsoft.com/en-us/windows/win32/api/wingdi/ns-wingdi-bitmapinfo
// https://docs.microsoft.com/en-us/windows/win32/gdi/bitmap-storage
UINT NTAPI GDI_WriteBitmap(HDC DC, HBITMAP Bitmap, PVOID Buffer) {
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
    if (Buffer) {
        pbmfh = (PBITMAPFILEHEADER)Buffer;
        pbmfh->bfType = 'MB';
        pbmfh->bfOffBits = uHeadersSize;
        pbmfh->bfSize = uFileSize;
        pbmfh->bfReserved1 = pbmfh->bfReserved2 = 0;
        pbmi = ADD_OFFSET(pbmfh, sizeof(*pbmfh), BITMAPINFO);
        pbmi->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
        pbmi->bmiHeader.biWidth = bmp.bmWidth;
        pbmi->bmiHeader.biHeight = bmp.bmHeight;
        pbmi->bmiHeader.biPlanes = bmp.bmPlanes;
        pbmi->bmiHeader.biBitCount = bmp.bmBitsPixel;
        pbmi->bmiHeader.biSizeImage = uImageSize;
        pbmi->bmiHeader.biClrUsed = dwClrItem;
        pbmi->bmiHeader.biCompression = BI_RGB;
        pbmi->bmiHeader.biXPelsPerMeter = pbmi->bmiHeader.biYPelsPerMeter = pbmi->bmiHeader.biClrImportant = 0;
        pBits = ADD_OFFSET(pbmi, pbmi->bmiHeader.biSize + (DWORD_PTR)dwClrSize, VOID);
        if (!GetDIBits(DC, Bitmap, 0, bmp.bmHeight, pBits, pbmi, DIB_RGB_COLORS))
            return 0;
    }
    return uFileSize;
}

VOID NTAPI GDI_InitInternalFontInfo(PENUMLOGFONTEXDVW InternalFontInfo) {
    InternalFontInfo->elfEnumLogfontEx.elfFullName[0] = InternalFontInfo->elfEnumLogfontEx.elfStyle[0] = InternalFontInfo->elfEnumLogfontEx.elfScript[0] = '\0';
    InternalFontInfo->elfDesignVector.dvReserved = STAMP_DESIGNVECTOR;
    InternalFontInfo->elfDesignVector.dvNumAxes = 0;
}

VOID NTAPI GDI_SetFontInfo(PLOGFONTW FontInfo, LONG FontSize, LONG FontWeight, PCWSTR FontName) {
    FontInfo->lfHeight = FontSize;
    FontInfo->lfWidth = 0;
    FontInfo->lfEscapement = 0;
    FontInfo->lfOrientation = 0;
    FontInfo->lfWeight = FontWeight;
    FontInfo->lfItalic = FALSE;
    FontInfo->lfUnderline = FALSE;
    FontInfo->lfStrikeOut = FALSE;
    FontInfo->lfCharSet = DEFAULT_CHARSET;
    FontInfo->lfOutPrecision = OUT_TT_PRECIS;
    FontInfo->lfClipPrecision = CLIP_DEFAULT_PRECIS;
    FontInfo->lfQuality = CLEARTYPE_QUALITY;
    FontInfo->lfPitchAndFamily = DEFAULT_PITCH | FF_DONTCARE;
    if (FontName == NULL)
        FontInfo->lfFaceName[0] = '\0';
    else
        Str_CchCopyW(FontInfo->lfFaceName, FontName);
}

HFONT NTAPI GDI_CreateFont(LONG FontSize, LONG FontWeight, PCWSTR FontName) {
    ENUMLOGFONTEXDVW stEnumLogFontExDesignVectorW;
    GDI_InitInternalFontInfo(&stEnumLogFontExDesignVectorW);
    GDI_SetFontInfo(&stEnumLogFontExDesignVectorW.elfEnumLogfontEx.elfLogFont, FontSize, FontWeight, FontName);
    return CreateFontIndirectExW(&stEnumLogFontExDesignVectorW);
}

INT NTAPI GDI_GetFont(HFONT Font, PLOGFONTW FontInfo) {
    return GetObjectW(Font, sizeof(*FontInfo), FontInfo);
}

BOOL NTAPI GDI_DrawIcon(HDC DC, HICON Icon, INT X, INT Y, INT CX, INT CY) {
    ICONINFO    stIconInfo;
    BITMAP      stBmp;
    RECT        rcDst;
    INT         iCX, iCY, iIconHeight;
    BOOL        bRet = FALSE;

    if (!GetIconInfo(Icon, &stIconInfo))
        return FALSE;

    if (GetObject(stIconInfo.hbmMask, sizeof(stBmp), &stBmp)) {
        iIconHeight = stIconInfo.hbmColor ? stBmp.bmHeight : (stBmp.bmHeight / 2);
        if (CX && CY) {
            iCX = CX;
            iCY = CY;
        } else {
            iCX = stBmp.bmWidth;
            iCY = iIconHeight;
        }

        HDC     hDC;
        INT     iPrevMode;
        hDC = CreateCompatibleDC(DC);
        SelectBitmap(hDC, stIconInfo.hbmMask);
        iPrevMode = SetStretchBltMode(DC, HALFTONE);
        rcDst.left = X;
        rcDst.top = Y;
        rcDst.right = X + iCX;
        rcDst.bottom = Y + iCY;
        GDI_FillSolidRect(DC, &rcDst, RGB(255, 255, 255));

        if (StretchBlt(DC, X, Y, iCX, iCY, hDC, 0, 0, stBmp.bmWidth, iIconHeight, SRCAND))
            if (stIconInfo.hbmColor) {
                SelectBitmap(hDC, stIconInfo.hbmColor);
                if (StretchBlt(DC, X, Y, iCX, iCY, hDC, 0, 0, stBmp.bmWidth, iIconHeight, SRCAND))
                    bRet = TRUE;
            } else {
                if (StretchBlt(DC, X, Y, iCX, iCY, hDC, 0, iIconHeight, stBmp.bmWidth, iIconHeight, SRCINVERT))
                    bRet = TRUE;
            }
        SetStretchBltMode(DC, iPrevMode);
        DeleteDC(hDC);
    }

    DeleteObject(stIconInfo.hbmMask);
    if (stIconInfo.hbmColor)
        DeleteObject(stIconInfo.hbmColor);
    return bRet;
}

BOOL NTAPI GDI_CreateSnapshot(HWND Window, PGDI_SNAPSHOT Snapshot) {
    HDC     hDC;
    BOOL    bRet;
    if (Window) {
        RECT rc;
        GetClientRect(Window, &rc);
        Snapshot->Position.x = Snapshot->Position.y = 0;
        Snapshot->Size.cx = rc.right;
        Snapshot->Size.cy = rc.bottom;
    } else
        UI_GetScreenPos(&Snapshot->Position, &Snapshot->Size);
    hDC = GetDC(Window);
    Snapshot->DC = CreateCompatibleDC(hDC);
    Snapshot->Bitmap = CreateCompatibleBitmap(hDC, Snapshot->Size.cx, Snapshot->Size.cy);
    Snapshot->OriginalBitmap = SelectObject(Snapshot->DC, Snapshot->Bitmap);
    bRet = BitBlt(Snapshot->DC, 0, 0, Snapshot->Size.cx, Snapshot->Size.cy, hDC, Snapshot->Position.x, Snapshot->Position.y, SRCCOPY);
    ReleaseDC(Window, hDC);
    return bRet;
}

BOOL NTAPI GDI_DeleteSnapshot(PGDI_SNAPSHOT Snapshot) {
    BOOL bRet;
    bRet = DeleteDC(Snapshot->DC);
    bRet &= DeleteObject(Snapshot->Bitmap);
    bRet &= DeleteObject(Snapshot->OriginalBitmap);
    return bRet;
}
