#include <stdio.h>
#include "..\NTAssassin\include\NTAssassin\NTAssassin.h"

#pragma comment (lib, "NTAssassin.lib")

int main() {
    WCHAR szRGB[HEXRGB_CCH];
    Str_RGBToHexExW(RGB(255, 0, 0), szRGB, ARRAYSIZE(szRGB));
    return 0;
}