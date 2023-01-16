#include "include\NTAssassin\NTATime.h"

ULONGLONG NTAPI Time_StopWatchEx(ULONGLONG PrevTime, ULONG Multiplier)
{
    LARGE_INTEGER liCounter, liFreq;
    NtQueryPerformanceCounter(&liCounter, &liFreq);
    return liCounter.QuadPart * Multiplier / liFreq.QuadPart - PrevTime;
}

VOID NTAPI Time_UnixTimeToFileTime(time_t UnixTime, PFILETIME FileTime)
{
    ULARGE_INTEGER Time;
    Time.QuadPart = (UnixTime * 10000000LL) + 116444736000000000LL;
    FileTime->dwLowDateTime = Time.LowPart;
    FileTime->dwHighDateTime = Time.HighPart;
}
