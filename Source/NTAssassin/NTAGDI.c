#include "NTAssassin.h"

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
NTSTATUS NTAPI GDI_WriteBitmap(HDC DC, HBITMAP Bitmap, PVOID Buffer, SIZE_T BufferSize, PSIZE_T BytesWritten) {
    BITMAP              bmp;
    ULONG               cClrBits;
    BITMAPFILEHEADER    bmfh;
    BITMAPINFOHEADER    bmih;
    LPBITMAPINFO        lpbmi;
    DWORD               dwClrItem, dwClrSize;
    LPRGBQUAD           lpColor = NULL;
    LPBYTE              pBuff;
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
    // Allocate color table if needed
    if (cClrBits < 24) {
        dwClrItem = 1 << cClrBits;
        dwClrSize = dwClrItem * sizeof(RGBQUAD);
        lpColor = Mem_HeapAllocEx(HEAP_ZERO_MEMORY, dwClrSize);
        if (!lpColor)
            return STATUS_NO_MEMORY;
    } else
        dwClrItem = dwClrSize = 0;
    // Construct BITMAPINFOHEADER by BITMAP
    bmih.biSize = sizeof(BITMAPINFOHEADER);
    bmih.biWidth = bmp.bmWidth;
    bmih.biHeight = bmp.bmHeight;
    bmih.biPlanes = bmp.bmPlanes;
    bmih.biBitCount = bmp.bmBitsPixel;
    bmih.biSizeImage = BYTE_ALIGN(bmih.biWidth * cClrBits, 32) / 8 * bmih.biHeight;
    bmih.biClrUsed = dwClrItem;
    bmih.biCompression = BI_RGB;
    bmih.biXPelsPerMeter = bmih.biYPelsPerMeter = bmih.biClrImportant = 0;
    // Construct BITMAPFILEHEADER by BITMAPINFOHEADER
    bmfh.bfType = 'MB';
    bmfh.bfOffBits = sizeof(BITMAPFILEHEADER) + bmih.biSize + dwClrSize;
    bmfh.bfSize = bmfh.bfOffBits + bmih.biSizeImage;
    bmfh.bfReserved1 = bmfh.bfReserved2 = 0;
    // Write file data
    if (Buffer) {
        // Write BITMAPFILEHEADER and BITMAPINFOHEADER
        pBuff = Buffer;
        RtlMoveMemory(pBuff, &bmfh, sizeof(BITMAPFILEHEADER));
        pBuff = ADD_OFFSET(pBuff, sizeof(BITMAPFILEHEADER), BYTE);
        lpbmi = (LPBITMAPINFO)pBuff;
        RtlMoveMemory(pBuff, &bmih, sizeof(BITMAPINFOHEADER));
        pBuff = ADD_OFFSET(pBuff, sizeof(BITMAPINFOHEADER), BYTE);
        // Write RGBQUADs
        if (dwClrItem) {
            RtlMoveMemory(pBuff, lpColor, dwClrSize);
            pBuff = ADD_OFFSET(pBuff, dwClrSize, BYTE);
            Mem_HeapFree(lpColor);
        }
        // Write DIB
        if (!GetDIBits(DC, Bitmap, 0, bmp.bmHeight, pBuff, lpbmi, DIB_RGB_COLORS))
            return STATUS_INVALID_PARAMETER;
    }
    if (BytesWritten)
        *BytesWritten = bmfh.bfSize;
    return STATUS_SUCCESS;
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