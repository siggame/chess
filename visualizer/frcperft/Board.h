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
#ifndef BOARD_H_
#define BOARD_H_

#include "common.h"
#include "Side.h"
#include "Move.h"

#include <stdio.h>

//the data that is copied back (=not recalculated) when undoing a move.
struct Undo {
    Bitboard attacked;
    Bitboard pinned;
    int16    fifty;
    int16    ply;
    uchar    castling;
    uchar    epsq;
};


/*
 * represents a chess board with pieces
 * is able to generate,make en undo moves
 */
class align64 Board {
    Side     _side[2]; //[color ] the bitboard armies of White and Black
    uchar    _psq[64]; //[square] contains the piece that sits on a square
    Bitboard _occupied;//contains all occupied squares
    Undo     _info;    //undo information
    int      _stm;     //side to move

__frc__(
    //castling information for FRC
    CastleInfo _castleinfo[2][2]; //[color][iskingside]
    uchar      _castlemask[64];   //[square]
)

    //debugging suport
    void verify(int stm, const char *desc);
    void fail  (int stm, const char *desc=0, const char *cause=0, Move move=0);

    //utilities
    char xfencastlechar(char c, int stm) const;
    void initcastling(int stm, int king, int qrook, int krook);
    void addpiece    (int stm, int piece, int sq);
    Bitboard genpinned   (int stm) const;
    Bitboard genattacks  (int stm) const;
    Bitboard genattackers(int stm, int sq) const;

    //move generation support
    Move *genepcaptures  (Move *moves) const;
    Move *genevasions    (Move *moves) const;
    template<bool generate>
    Move *gennormalmoves (Move *moves) const;

    //move/undo support
    void updateinfo() {
        _info.pinned   = genpinned (_stm);
        _info.attacked = genattacks(_stm^1);
    }
    void undospecial(Move move);
    void movespecial(Move move);

public:
    const Side &side (int stm) { return _side[ stm]; }
    const Side &side (       ) { return _side[_stm]; }
    const Side &enemy(       ) { return _side[_stm^1]; }

    //info
    int  stm      (      ) const { return _stm; }
    int  ply      (      ) const { return _info.ply; }
    int  fifty    (      ) const { return _info.ply-_info.fifty; }
    bool ischeck  (      ) const { return _info.attacked & _side[_stm].kings;}
    int  piece    (int sq) const { return _psq[sq]; }
    char piecechar(int sq) const { return "?PNBRQK"[piece(sq)&7]; }
    const Undo & save(   ) const { return _info; }

    //setup
    void clear();
    void fen(char fen[256]) const;
    bool setfen(const char *fen);
    void setstartpos() { setfen("rnbqkbnr/pppppppp/////PPPPPPPP/RNBQKBNR w KQkq - 0 1"); }

    //move generation and counting
    Move *genmoves(Move *moves) {
        return ischeck()?genevasions(moves):gennormalmoves<true>(moves);
    }
    uint64 countmoves(Move *moves) {
        return (ischeck()?genevasions(moves):gennormalmoves<false>(moves))-moves;
    }

    //move/undo
    void move(Move move);
    void undo(Move move, const Undo &undo);

    //logging & debugging
    void print(FILE *file) const;
    void print() const { print(stdout); }
    void verify(const char *desc, Move move=0);
}
;

#endif
