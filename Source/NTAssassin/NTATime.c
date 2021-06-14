#include "NTAssassin\NTAssassin.h"

ULONGLONG NTAPI Time_StopWatch100nm(ULONGLONG PrevTime) {
    LARGE_INTEGER   li;
    NtQuerySystemTime(&li);
    return li.QuadPart - PrevTime;
}

ULONGLONG NTAPI Time_StopWatch16ms(ULONGLONG PrevTime) {
    ULONGLONG ullTime = GetTickCount64() >> 4;
    return ullTime - PrevTime;
}

ULONGLONG NTAPI Time_StopWatch1ms(ULONGLONG PrevTime) {
    LARGE_INTEGER   liCounter, liFreq;
    QueryPerformanceCounter(&liCounter);
    QueryPerformanceFrequency(&liFreq);
    return liCounter.QuadPart * 1000 / liFreq.QuadPart - PrevTime;
}

ULONGLONG NTAPI Time_StopWatch1us(ULONGLONG PrevTime) {
    LARGE_INTEGER   liCounter, liFreq;
    QueryPerformanceCounter(&liCounter);
    QueryPerformanceFrequency(&liFreq);
    return liCounter.QuadPart * 1000000 / liFreq.QuadPart - PrevTime;
}