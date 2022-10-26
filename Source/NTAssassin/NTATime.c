#include "include\NTAssassin\NTATime.h"

ULONGLONG NTAPI Time_StopWatch100nm(ULONGLONG PrevTime) {
    LARGE_INTEGER li;
    NtQuerySystemTime(&li);
    return li.QuadPart - PrevTime;
}

ULONGLONG NTAPI Time_StopWatch16ms(ULONGLONG PrevTime) {
    ULONGLONG ullTime = GetTickCount64() >> 4;
    return ullTime - PrevTime;
}

ULONGLONG NTAPI Time_StopWatch1ms(ULONGLONG PrevTime) {
    LARGE_INTEGER liCounter, liFreq;
    NtQueryPerformanceCounter(&liCounter, &liFreq);
    return liCounter.QuadPart * 1000 / liFreq.QuadPart - PrevTime;
}

ULONGLONG NTAPI Time_StopWatch1us(ULONGLONG PrevTime) {
    LARGE_INTEGER liCounter, liFreq;
    NtQueryPerformanceCounter(&liCounter, &liFreq);
    return liCounter.QuadPart * 1000000 / liFreq.QuadPart - PrevTime;
}

VOID NTAPI Time_UnixTimeToFileTime(time_t UnixTime, PFILETIME FileTime) {
    ULARGE_INTEGER Time;
    Time.QuadPart = (UnixTime * 10000000LL) + 116444736000000000LL;
    FileTime->dwLowDateTime = Time.LowPart;
    FileTime->dwHighDateTime = Time.HighPart;
}
