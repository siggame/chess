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
#ifndef SIDE_H_
#define SIDE_H_

#include "chess.h"
#include "Bitboard.h"

//struct representing a side of the board: white or black
struct align64 Side {
//"class with constructor not allowed within union"
//union { struct {
    Bitboard  occupied;
    Bitboard  pawns;
    Bitboard  knights;
    Bitboard  bishops;
    Bitboard  rooks;
    Bitboard  queens;
    Bitboard  kings; //}; Bitboard bb[7]; };
    int       king;

    //if sq is empty: adds, otherwise removes a piece
    void flip(int piece, int sq) {
        assert(sq   >=0 && sq   <64);
        assert(piece>=0 && piece< 8);

        Bitboard flip = Bitboard::bit(sq);
        occupied ^=flip;
        (&occupied)[piece]^=flip;
//      bb[type] ^=flip;
    }

    //moves a piece to an other (or the same) square
    void move(int piece, int from, int to) {
        assert(from >=0 && from<64);
        assert(to   >=0 &&   to<64);
        assert(piece>=0 && piece<8);

        Bitboard flip = Bitboard::bit(from)^Bitboard::bit(to);
        occupied ^=flip;
        (&occupied)[piece] ^=flip;
//      bb[type] ^=flip;
    }
    Bitboard bishopsqueens() const { return bishops|queens; }
    Bitboard   rooksqueens() const { return rooks  |queens; }
    Bitboard pieces( ) const { return occupied^pawns; }
    Bitboard minors () const { return bishops|knights; }
    Bitboard majors () const { return rooks|queens; }
    Bitboard sliders() const { return rooks|queens|bishops; }
};

#endif
