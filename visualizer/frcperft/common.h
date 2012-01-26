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
#ifndef COMMON_H_
#define COMMON_H_

//system info
//NOTE: be careful: MSVC gives wrong results when NO space after defined, e.g.
//      #define WINDOWS  (defined(_WIN32) || defined(_WIN64))
#define WINDOWS  (defined _WIN32 || defined _WIN64)
#define APPLE    (defined __APPLE__)
#define UNIX     (defined __unix__ || defined __unix)
#define SYSTEM64 (defined _LP64    || defined __LP64__ || defined __x86_64__ || defined _WIN64)
#define SYSTEM32 (!SYSTEM64)


#if defined __SSE4_2__
#define CPU_SSE42 1
#else
#define CPU_SSE42 0
#endif


//common types
typedef unsigned char uchar;
typedef unsigned int  uint ;


//compiler abstractions for types, alignment and branch prediction
#if defined __GNUC__ || defined __GNUG__
//GNU g++

#include <stdint.h>
typedef  int8_t   int8;
typedef uint8_t  uint8;
typedef  int16_t  int16;
typedef uint16_t uint16;
typedef  int32_t  int32;
typedef uint32_t uint32;
typedef  int64_t  int64;
typedef uint64_t uint64;

#define align4   __attribute__((aligned(  4)))
#define align8   __attribute__((aligned(  8)))
#define align16  __attribute__((aligned( 16)))
#define align32  __attribute__((aligned( 32)))
#define align64  __attribute__((aligned( 64)))
#define align128 __attribute__((aligned(128)))

#define   likely(x) (__builtin_expect(!!(x), 1))
#define unlikely(x) (__builtin_expect(!!(x), 0))
#define if1(x) if(__builtin_expect(!!(x),1))
#define if0(x) if(__builtin_expect(!!(x),0))


#elif defined _MSC_VER
//MS visual c++

typedef          __int8    int8;
typedef unsigned __int8   uint8;
typedef          __int16   int16;
typedef unsigned __int16  uint16;
typedef          __int32   int32;
typedef unsigned __int32  uint32;
typedef          __int64   int64;
typedef unsigned __int64  uint64;

#define align4   __declspec(align(  4))
#define align8   __declspec(align(  8))
#define align16  __declspec(align( 16))
#define align32  __declspec(align( 32))
#define align64  __declspec(align( 64))
#define align128 __declspec(align(128))

#define   likely(x) (x)
#define unlikely(x) (x)
#define if1(x) if(x)
#define if0(x) if(x)

#else

#warning unknown compiler

#include <stdint.h>
typedef  int8_t   int8;
typedef uint8_t  uint8;
typedef  int16_t  int16;
typedef uint16_t uint16;
typedef  int32_t  int32;
typedef uint32_t uint32;
typedef  int64_t  int64;
typedef uint64_t uint64;

#define align4
#define align8
#define align16
#define align32
#define align64
#define align128

#define   likely(x)  (x)
#define unlikely(x)  (x)
#define if1(x) if(x)
#define if0(x) if(x)
#endif

#endif
