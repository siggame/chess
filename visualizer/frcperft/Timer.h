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
#ifndef TIMER_H_
#define TIMER_H_

#include "common.h"
#include "asm.h"
#include <stdio.h>
#include <time.h>

/*
 * Calculates elapsed cpu cycles (using rdtc) and elapsed wall time
 * between calling start() and stop().
 *
 * NOTE: cpucycles may be inaccurate on multicore systems (Google RDTSC)
 */
class Timer {
    clock_t _starttime;
    clock_t _stoptime;
    uint64 _startticks;
    uint64 _stopticks;

public:
    Timer() {}

    void start() { _starttime= clock(); _startticks= _asm_::rdtsc(); }
    void stop () { _stopticks= _asm_::rdtsc(); _stoptime  = clock(); }

    //elapsed ticks, seconds
    uint64 ticks  () { return _stopticks-_startticks; }
    double seconds() { return (_stoptime-_starttime)/double(CLOCKS_PER_SEC); }

    void log(uint64 n, const char *desc) {
        double zeconds = seconds();
        double opsms   = zeconds>0?n/(1000*zeconds):0;
        double ticksop = ticks()/double(n);
        printf("%-20s  %10.0f ops,  %7.3f seconds,  %7.0f ops/ms,  %6.1f ticks/op\n",
               desc,    double(n),        zeconds,         opsms,  ticksop);
    }
}
;
#endif
