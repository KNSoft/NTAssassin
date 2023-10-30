#pragma once

#include "NTAssassin_Base.h"

/// <summary>
/// Gets time interval like stop watch
/// </summary>
/// <param name="PrevTime">Reference time to calcuate interval, or pass 0 to get a new one</param>
/// <param name="Multiplier">Multiplier converts us to other unit</param>
/// <returns>Time interval from PrevTime to now, or current time if PrevTime is 0</returns>
NTA_API ULONGLONG NTAPI Time_StopWatchEx(ULONGLONG PrevTime, ULONG Multiplier);
#define Time_StopWatch1ms(PrevTime) Time_StopWatchEx(PrevTime, 1000)
#define Time_StopWatch Time_StopWatch1ms

/// <summary>
/// Converts Unix timestamp to FILETIME
/// </summary>
/// <see href="https://learn.microsoft.com/en-us/windows/win32/sysinfo/converting-a-time-t-value-to-a-file-time">Converting a time_t value to a FILETIME - Microsoft Learn</see>
NTA_API VOID NTAPI Time_UnixTimeToFileTime(time_t UnixTime, PFILETIME FileTime);
