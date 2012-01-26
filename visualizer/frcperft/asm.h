/*
 * Copyright (C) 2008-2011 AJ Siemelink
 *
 * This code is provided 'as-is', without any express or implied warranty.
 * In no event will the author be held liable for any damages arising from
 * the use of this code.
 *
 * Permission is granted to anyone to use this code for non-commercial purposes.
 * This use includes compiling, altering and creating derivative work.
 *
 * Redistribution of the code in source or binary form is allowed,
 * providing that the following conditions are met:
 *
 * 1. This notice may not be removed or altered from any source distribution.
 *    This notice must be included with any binary distribution.
 *
 * 2. The origin of this code must not be misrepresented;
 *    you must not claim that you wrote the original code.
 *
 * 3. Altered versions must be plainly marked as such, and must
 *    not be misrepresented as being the original code.
 *
 * 4. The distribution, including any accompanying works,
 *    must be provided free of any charges and/or fees.
 */
#ifndef ASM_H_
#define ASM_H_

#include "common.h"

/*
 * compiler independent assembly code intrinsics.
 */
namespace _asm_ {

#ifdef _MSC_VER
#include <intrin.h>
    #pragma intrinsic(__rdtsc)
    static inline uint64 rdtsc() {
        return __rdtsc();
    }
    #pragma intrinsic(_BitScanForward)
    static inline unsigned long bsf(unsigned long mask) {
        unsigned long result;
        _BitScanForward(&result, mask);
        return result;
    }
#if SYSTEM64
    #pragma intrinsic(_BitScanForward64)
    static inline unsigned long bsfq(unsigned __int64 mask) {
        unsigned long result;
        _BitScanForward64(&result, mask);
        return result;
    }
    #pragma intrinsic(_BitScanReverse64)
    static inline unsigned long bsrq(unsigned __int64 mask) {
        unsigned long result;
        _BitScanReverse64(&result, mask);
        return result;
    }
    #pragma intrinsic(__popcnt64)
    static inline unsigned __int64 popcntq(unsigned __int64 mask) {
        return __popcnt64(mask);
    }
#endif


#elif defined __GNUC__
/*
 * NOTE: GCC ASM FORMAT:
 * __asm__ (
 *   "statements"
 *   : output operands      comma separated triplets: [asm-var] "constraints" (c-var)
 *   : input operands       comma separated triplets
 *   : clobbered registers
 *
 *   "constraints"
 *   "r"       register
 *   "a", "d", eax, edx register
 *   "="       output, write only
 *   "m"       memory address
 *   "i"       immediate value
 *   "I"       restrict to 0..31
 *   "J"       restrict to 0..63
 */
    static inline uint64 rdtsc() {
        uint32 hi, lo;
        __asm__ __volatile__ ("rdtsc" : "=a"(lo), "=d"(hi));
        return (uint64(hi)<<32)|lo;
    }

    static inline uint bsf(uint32 mask) {
        uint32 result;
        __asm__ (
            "bsf %[mask], %[result]"
            :[result] "=r" (result)
            :[mask  ] "mr" (mask  )
        );
        return result;
    }

#if SYSTEM64
    static inline uint64 bsfq(uint64 mask) {
        uint64 result;
        __asm__ (
            "bsfq %[mask], %[result]"
            :[result] "=r" (result)
            :[mask  ] "mr" (mask  )
        );
        return result;
    }
    static inline uint64 bsrq(uint64 mask) {
        uint64 result;
        __asm__ (
            "bsrq %[mask], %[result]"
            :[result] "=r" (result)
            :[mask  ] "mr" (mask  )
        );
        return result;
    }

#if CPU_SSE42
   static inline uint64 popcntq(const uint64 mask) {
        uint64 result;
        __asm__ (
            "popcntq %[mask], %[result]"
            :[result] "=r" (result)
            :[mask  ] "mr" (mask  )
        );
        return result;
    }
#endif  //sse42
#endif  //system64

#endif  //gcc
}       //namespace
#endif  //ASM_H_
