﻿#pragma once

#include "NTAssassin.h"

/**
  * @brief Rounds a value
  */
#define Math_RoundUInt(val) ((UINT)((val) + 0.5))
#define Math_RoundInt(val) ((INT)((val) + 0.5))
#define Math_RoundUIntPtr(val) ((INT_PTR)((val) + 0.5))
#define Math_RoundIntPtr(val) ((INT_PTR)((val) + 0.5))

/**
  * @brief Generates a random
  * @return Returns a random number in the range [0, MAXLONG-1]
  * @see "RtlRandomEx"
  */
NTA_API ULONG NTAPI Math_Random();

/**
  * @brief Generates a random in specified range
  * @param[in] Min Minimum
  * @param[in] Max Maximum
  * @return Returns a random number in the range [Min, Max]
  * @see "RtlRandomEx"
  */
NTA_API ULONG NTAPI Math_RangedRandom(ULONG Min, ULONG Max);

/**
  * @brief Checks the number is a power of 2
  */
#define Math_IsPowerOf2(n) ((n != 0) && ((n & (n - 1)) == 0))

#define Math_Abs(v1, v2) ((v1) > (v2) ? (v1) - (v2) : (v2) - (v1))