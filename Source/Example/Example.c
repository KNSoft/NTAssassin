#include <stdio.h>
#include "NTAssassin\NTAssassin.h"

#pragma comment (lib, "NTAssassin.lib")

int main() {
    while (TRUE) {
        WCHAR   szInput[4096];
        UINT8  ullOutput;
        BOOL    bSucc;
        _getws_s(szInput, ARRAYSIZE(szInput));
        bSucc = Str_ToUIntW(szInput, &ullOutput);
        if (!bSucc)
            ullOutput = 0;
        wprintf(L"Ret: %u, Value: %u \r\n", bSucc, ullOutput);
    }
    return 0;
}