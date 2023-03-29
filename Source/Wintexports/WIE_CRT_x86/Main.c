/*
 * ISA_AVAILABLE
 * 
 * FIXME:
 *   Regression, we cannot initialize __isa_available
 *   at startup, add a TLS callback procedure?
 * 
 */

#include <isa_availability.h>
int __isa_available = __ISA_AVAILABLE_SSE2;

/*
 * _except1
 * 
 * _except1 is not implemented and not be used,
 * call __fastfail instead.
 */

#include <intrin.h>

#define FAST_FAIL_INVALID_EXCEPTION_CHAIN 21

_declspec(naked) _except1()
{
    __fastfail(FAST_FAIL_INVALID_EXCEPTION_CHAIN);
}
