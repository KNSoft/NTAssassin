#pragma once

// Windows NT

#define STRSAFE_MAX_CCH			2147483647
#define STRSAFE_MAX_LENGTH		(STRSAFE_MAX_CCH - 1)
#define PROCESSOR_FEATURE_MAX	64
#define MM_SHARED_USER_DATA_VA	0x7FFE0000

// NTSTATUS

#undef STATUS_WAIT_0 
#undef STATUS_ABANDONED_WAIT_0
#undef STATUS_USER_APC
#undef STATUS_TIMEOUT
#undef STATUS_PENDING
#undef DBG_EXCEPTION_HANDLED
#undef DBG_CONTINUE
#undef STATUS_SEGMENT_NOTIFICATION
#undef STATUS_FATAL_APP_EXIT
#undef DBG_REPLY_LATER
#undef DBG_TERMINATE_THREAD
#undef DBG_TERMINATE_PROCESS
#undef DBG_CONTROL_C
#undef DBG_PRINTEXCEPTION_C
#undef DBG_RIPEXCEPTION
#undef DBG_CONTROL_BREAK
#undef DBG_COMMAND_EXCEPTION
#undef DBG_PRINTEXCEPTION_WIDE_C
#undef STATUS_GUARD_PAGE_VIOLATION
#undef STATUS_DATATYPE_MISALIGNMENT
#undef STATUS_BREAKPOINT
#undef STATUS_SINGLE_STEP
#undef STATUS_LONGJUMP
#undef STATUS_UNWIND_CONSOLIDATE
#undef DBG_EXCEPTION_NOT_HANDLED
#undef STATUS_ACCESS_VIOLATION
#undef STATUS_IN_PAGE_ERROR
#undef STATUS_INVALID_HANDLE
#undef STATUS_INVALID_PARAMETER
#undef STATUS_NO_MEMORY
#undef STATUS_ILLEGAL_INSTRUCTION
#undef STATUS_NONCONTINUABLE_EXCEPTION
#undef STATUS_INVALID_DISPOSITION
#undef STATUS_ARRAY_BOUNDS_EXCEEDED
#undef STATUS_FLOAT_DENORMAL_OPERAND
#undef STATUS_FLOAT_DIVIDE_BY_ZERO
#undef STATUS_FLOAT_INEXACT_RESULT
#undef STATUS_FLOAT_INVALID_OPERATION
#undef STATUS_FLOAT_OVERFLOW
#undef STATUS_FLOAT_STACK_CHECK
#undef STATUS_FLOAT_UNDERFLOW
#undef STATUS_INTEGER_DIVIDE_BY_ZERO
#undef STATUS_INTEGER_OVERFLOW
#undef STATUS_PRIVILEGED_INSTRUCTION
#undef STATUS_STACK_OVERFLOW
#undef STATUS_DLL_NOT_FOUND
#undef STATUS_ORDINAL_NOT_FOUND
#undef STATUS_ENTRYPOINT_NOT_FOUND
#undef STATUS_CONTROL_C_EXIT
#undef STATUS_DLL_INIT_FAILED
#undef STATUS_FLOAT_MULTIPLE_FAULTS
#undef STATUS_FLOAT_MULTIPLE_TRAPS
#undef STATUS_REG_NAT_CONSUMPTION
#undef STATUS_HEAP_CORRUPTION
#undef STATUS_STACK_BUFFER_OVERRUN
#undef STATUS_INVALID_CRUNTIME_PARAMETER
#undef STATUS_ASSERTION_FAILURE
#undef STATUS_ENCLAVE_VIOLATION
#undef STATUS_INTERRUPTED
#undef STATUS_THREAD_NOT_RUNNING
#undef STATUS_ALREADY_REGISTERED
#undef STATUS_SXS_EARLY_DEACTIVATION
#undef STATUS_SXS_INVALID_DEACTIVATION
#undef STATUS_CONTROL_STACK_VIOLATION

#include <ntstatus.h>

// Patches C26454 warning

#undef NM_FIRST
#define NM_FIRST 4294967296

#undef TCN_FIRST
#define TCN_FIRST 4294966746

#undef LVN_ITEMCHANGING
#define LVN_ITEMCHANGING 4294967196

#undef LVN_ITEMCHANGED
#define LVN_ITEMCHANGED 4294967195