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
#ifndef CHESS_H_
#define CHESS_H_

#include "common.h"

/*
 * FRC
 * ----------------------------------------------------------------------------
 * Fisher random chess, or chess960, is a chess variant in which the pieces
 * start out on one of 960 different positions in the back rank.
 * See http://en.wikipedia.org/wiki/Chess960 for detailed information.
 *
 * perft intends to fully support FRC:
 * - accept and print X-fen chess positions
 * - accept KxR-style castle move notation
 * - generate moves according to the FRC castling rules
 *
 * 0 disables FRC support
 * 1 enables FRC support
 */
#define FRC 1

#if FRC
#define __frc__(x) x
#else
#define __frc__(x)
#endif

//maximum #moves for any position
static const int MaxMoves = 256;

//constants for the squares of the board
enum   {A1=0,B1,C1,D1,E1,F1,G1,H1,
          A2,B2,C2,D2,E2,F2,G2,H2,
          A3,B3,C3,D3,E3,F3,G3,H3,
          A4,B4,C4,D4,E4,F4,G4,H4,
          A5,B5,C5,D5,E5,F5,G5,H5,
          A6,B6,C6,D6,E6,F6,G6,H6,
          A7,B7,C7,D7,E7,F7,G7,H7,
          A8,B8,C8,D8,E8,F8,G8,H8,
};

//masks for files and ranks
static const uint64 FileA = 0x0101010101010101ULL;
static const uint64 FileB = FileA<<1;
static const uint64 FileC = FileB<<1;
static const uint64 FileD = FileC<<1;
static const uint64 FileE = FileD<<1;
static const uint64 FileF = FileE<<1;
static const uint64 FileG = FileF<<1;
static const uint64 FileH = FileG<<1;

static const uint64 Rank1 = 0xffULL;
static const uint64 Rank2 = Rank1<<8;
static const uint64 Rank3 = Rank2<<8;
static const uint64 Rank4 = Rank3<<8;
static const uint64 Rank5 = Rank4<<8;
static const uint64 Rank6 = Rank5<<8;
static const uint64 Rank7 = Rank6<<8;
static const uint64 Rank8 = Rank7<<8;


//colors, pieces and castling rights
enum { White=0, Black=1};
enum { Empty=0, Pawn=1, Knight=2, Bishop=3, Rook=4, Queen=5, King=6};
enum { CastleWk=1, CastleBk=2, CastleWq=4, CastleWkq=5, CastleBq=8, CastleBkq=10 };
struct CastleInfo {
    uint64 empty, safe;
    uchar  kfrom, kto;
    uchar  rfrom, rto;
};

//macros for squares
#define BIT(sq) uint64(1ull<<(sq))
static inline int   x64(int sq)       { return sq&7;  }
static inline int   y64(int sq)       { return sq>>3; }
static inline int  sq64(int x, int y) { return (y<<3)+x; }
static inline bool ispromotionsq(int sq) { return ((unsigned)sq-8)>=48;}
static inline bool iskingside   (int sq) { return x64(sq)>=4; }

#endif
