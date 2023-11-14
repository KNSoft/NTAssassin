// I/O a file or device

#pragma once

#include "NTAssassin_Base.h"

/// <summary>
/// Writes data to specified file or device
/// </summary>
/// <param name="FileHandle">Handle to the file</param>
/// <param name="ByteOffset">Offset to write</param>
/// <param name="Buffer">Data to write</param>
/// <param name="Length">Size of data in bytes</param>
/// <returns>TRUE if succeeded, or NULL if failed, error code storaged in last STATUS</returns>
NTA_API BOOL NTAPI IO_Write(HANDLE FileHandle, ULONGLONG ByteOffset, _In_reads_bytes_(Length) PVOID Buffer, ULONG Length);
