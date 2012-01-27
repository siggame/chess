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
#ifndef MOVE_H_
#define MOVE_H_

#include "common.h"
#include "chess.h"

/*
 * represents a chess move.
 */
class Move {
private:
    //
    //    0.. 7  from
    //    8..15  to
    //   16..23  captured  piece
    //   24..26  promotion piece
    //   27      en passent flag
    //   28      castle flag
    //   29..31  unused
    uint _data;

public:
    static const uint Promotion   =      7<<24;
    static const uint PromoQueen  = Queen <<24;
    static const uint PromoRook   = Rook  <<24;
    static const uint PromoBishop = Bishop<<24;
    static const uint PromoKnight = Knight<<24;

    static const uint Capture= 255<<16;

    static const uint Ep     = 1<<27;
    static const uint Castle = 1<<28;

public:
    Move() {}
    Move(uint data)                  { _data = data; }
    Move(int from, int to         )  { _data=to|(from<<8); }
    Move(int from, int to, int cap)  { _data=to|(from<<8)|(cap<<16); }

    operator uint()  const { return _data; }

    static Move create    (int from) { return from<<8; }
    Move gencapture(int to, int cap) { return _data|to|(cap<<16); }

    int  to         () const { return (_data    )&255; }
    int  from       () const { return (_data>> 8)&255; }
    int  capture    () const { return (_data>>16)&255; }
    int  promotion  () const { return (_data>>24)&7  ; }

    bool isPlain    () const { return (_data&(Ep|Capture|Promotion))==0; }
    bool isCapture  () const { return _data&Capture; }
    bool isEp       () const { return _data&Ep; }
    bool isCastle   () const { return _data&Castle; }
    bool isSpecial  () const { return _data&(Ep|Castle|Promotion); }
    bool isPromotion() const { return _data&Promotion; }
}
;
#endif
