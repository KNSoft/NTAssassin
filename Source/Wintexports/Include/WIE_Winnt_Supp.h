/* Include this file before and after winnt.h */

#ifdef _WINNT_

#undef PSID
typedef SID* PSID;

#else

#define PSID MS_PSID

#endif
