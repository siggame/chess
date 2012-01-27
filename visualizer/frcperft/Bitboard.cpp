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
#include "Bitboard.h"

#include "chess.h"
#include <stdio.h>

#if EXTRACT_DEBRUIJN
//magic numbers for debruijn bitscan
//See: http://chessprogramming.wikispaces.com/BitScan#DeBruijnMultiplation
align64 const uint Bitboard::Magic[64] = {
    63,  0, 58,  1, 59, 47, 53,  2,
    60, 39, 48, 27, 54, 33, 42,  3,
    61, 51, 37, 40, 49, 18, 28, 20,
    55, 30, 34, 11, 43, 14, 22,  4,
    62, 57, 46, 52, 38, 26, 32, 41,
    50, 36, 17, 19, 29, 10, 13, 21,
    56, 45, 25, 31, 35, 16,  9, 12,
    44, 24, 15,  8, 23,  7,  6,  5,
};
#endif
#if EXTRACT_MAGIC
//magic numbers for Matt Taylor's bitscan (not equal to those on chess wiki)
align64 const uint Bitboard::Magic[64] = {
	 0, 31,  4, 33, 60, 15, 12, 34,
    61, 25, 51, 10, 56, 20, 22, 35,
    62, 30,  3, 54, 52, 24, 42, 19,
    57, 29,  2, 44, 47, 28,  1, 36,
    63, 32, 59,  5,  6, 50, 55,  7,
    16, 53, 13, 41,  8, 43, 46, 17,
    26, 58, 49, 14, 11, 40,  9, 45,
    21, 48, 39, 23, 18, 38, 37, 27,
};
#endif

#if BIT_LOOKUP
align64 uint64 Bitboard::Square[64];
#endif

align64 uint64 Bitboard::Between[64][64]; //32Kb line segment excluding from, excluding to
align64 uint64 Bitboard::Line   [64][64]; //32Kb full line containing two squares
align64 uint64 Bitboard::FileRank[2][64];
align64 uint64 Bitboard::Diagonal[2][64];
align64 uint64 Bitboard::Side  [64];

align64 uint64 Bitboard::Pawn[2][64];
align64 uint64 Bitboard::Knight [64];
align64 uint64 Bitboard::King   [64];
align64 uint64 Bitboard::Rook   [64];
align64 uint64 Bitboard::Bishop [64];
align64 uint64 Bitboard::Queen  [64];

//floodfill with king moves
Bitboard Bitboard::kflood() const {
    uint64 x = _data;
    x |= (x&~FileA)>>1;
    x |= (x&~FileH)<<1;

    return x|(x<<8)|(x>>8);
}

//floodfill with knight moves
Bitboard Bitboard::nflood() const {
    uint64 knights = _data;
    Bitboard side1 = (knights&~(FileH      ))<<1 | (knights&~(FileA      ))>>1;
    Bitboard side2 = (knights&~(FileH|FileG))<<2 | (knights&~(FileA|FileB))>>2;
    return side1<<16 | side1>>16 | side2>>8 | side2<<8;
}

void Bitboard::init() {

    initmagicmoves();

#if BIT_LOOKUP
	for (int i=0; i<64; i++) Square[i] = 1ull<<i;
#endif

	//orthogonal masks
	uint64 Xmask[8]={FileA,FileB,FileC,FileD,FileE,FileF,FileG,FileH};
	uint64 Ymask[8]={Rank1,Rank2,Rank3,Rank4,Rank5,Rank6,Rank7,Rank8};
    for (int sq=0; sq<64; sq++) {
        FileRank[0][sq] = Xmask[x64(sq)];
        FileRank[1][sq] = Ymask[y64(sq)];
    }

    //diagonal masks
    const uint64 a1h8 = 0x8040201008040201ULL;
    const uint64 h1a8 = 0x0102040810204080ULL;
    for (int sq=0; sq<64; sq++) {
        int x = x64(sq);
        int y = y64(sq);
        Diagonal[0][sq] = x>y?(a1h8>>((x-y)*8)):(a1h8<<(8*(y-x)));
        x=7-x;
        Diagonal[1][sq] = x>y?(h1a8>>((x-y)*8)):(h1a8<<(8*(y-x)));
    }

    //line masks
    for (int sq=0; sq<64; sq++) {
        int to;
        to = sq&~070; for (int y=0; y<8; y++) Line[sq][to+8*y] = FileRank[0][sq];
        to = sq&~7  ; for (int x=0; x<8; x++) Line[sq][to+1*x] = FileRank[1][sq];
        for (int to=sq+9; (to&7)!=0 && to<64; to+=9) Line[sq][to] = Diagonal[0][sq];
        for (int to=sq-9; (to&7)!=7 && to>=0; to-=9) Line[sq][to] = Diagonal[0][sq];
        for (int to=sq+7; (to&7)!=7 && to<64; to+=7) Line[sq][to] = Diagonal[1][sq];
        for (int to=sq-7; (to&7)!=0 && to>=0; to-=7) Line[sq][to] = Diagonal[1][sq];
    }

    //between
    for (int from=0;from<64;from++) {
        int d;
        d= 1; for (int to=from+d; to&7               ; to+=d) if (to>from+d) Between[from][to] = Between[from][to-d]|bit(to-d);
        d=-1; for (int to=from+d; (to+1)&7           ; to+=d) if (to<from+d) Between[from][to] = Between[from][to-d]|bit(to-d);
        d= 8; for (int to=from+d; to<64              ; to+=d) if (to>from+d) Between[from][to] = Between[from][to-d]|bit(to-d);
        d=-8; for (int to=from+d; to>=0              ; to+=d) if (to<from+d) Between[from][to] = Between[from][to-d]|bit(to-d);
        d= 9; for (int to=from+d; ( to   &7) && to<64; to+=d) if (to>from+d) Between[from][to] = Between[from][to-d]|bit(to-d);
        d= 7; for (int to=from+d; ((to+1)&7) && to<64; to+=d) if (to>from+d) Between[from][to] = Between[from][to-d]|bit(to-d);
        d=-7; for (int to=from+d; ( to   &7) && to>=0; to+=d) if (to<from+d) Between[from][to] = Between[from][to-d]|bit(to-d);
        d=-9; for (int to=from+d; ((to+1)&7) && to>=0; to+=d) if (to<from+d) Between[from][to] = Between[from][to-d]|bit(to-d);
    }

    //piece masks
    for (int sq=0; sq<64; sq++) {
        Bitboard sqmask = bit(sq);
        King  [sq] = sqmask.kflood();
        Knight[sq] = sqmask.nflood();
        Bishop[sq] = Diagonal[0][sq]|Diagonal[1][sq];
        Rook  [sq] = FileRank[0][sq]|FileRank[1][sq];
        Queen [sq] = Rook[sq]|Bishop[sq];
    }

    //pawns
    for (int sq=0; sq<64; sq++) {
        int x = x64(sq);
        if (sq<56 && x>0) Pawn[White][sq] |= bit(sq+7);
        if (sq<56 && x<7) Pawn[White][sq] |= bit(sq+9);
        if (sq> 7 && x<7) Pawn[Black][sq] |= bit(sq-7);
        if (sq> 7 && x>0) Pawn[Black][sq] |= bit(sq-9);

        if (x>0) Side[sq] |= bit(sq-1);
        if (x<7) Side[sq] |= bit(sq+1);
    }

//  kmask(E4).print("kmask(e4)");
//  kmask(A4).print("kmask(a4)");
//  kmask(H4).print("kmask(h4)");
//  nmask(E4).print("nmask(e4)");
//  nmask(A4).print("nmask(a4)");
//  nmask(G4).print("nmask(g4)");
}

void Bitboard::print(const char *desc) const {
    for (int y=7; y>=0; --y) {
        for (int x=0; x<8; x++) {
            printf(" %c", isset(sq64(x,y))?'x':'.');
        }
        printf("\n");
    }
    printf("%08x%08x %s\n", uint(_data>>32), uint(_data), desc);
}
