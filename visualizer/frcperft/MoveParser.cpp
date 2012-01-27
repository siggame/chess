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
 *    must be provided free of charges and/or fees.
 */
#include "MoveParser.h"
#include <string.h>
#include <stdio.h>

static const char *PieceChars="?PNBRQK"; //[Piece]

/*
 * parse string to a valid move.
 * return  0 in case the input does not identify a single move
 *
 * Allows multiple input formats, e.g.
 * pieces  : Nd3, Nxd3, Nexd3
 * pawns   : e4 e2e4, exd4 ed4, ed, e8Q, e8=Q, b2b1b
 * castling: O-O, Kg1, e1g1, Kxh1, e1h1
 */
Move MoveParser::parse(const char *smove) {

    //at least two characters
    if (!(smove && smove[0] && smove[1])) return 0;

    //phase I: collect info about from and to squares, pieces and castling
    int fromx=-1,fromy=-1,tox=-1,toy=-1;
    int piece=0,promo=0,castle=0;
    for (int i=0; smove[i]; i++) {
        char c = smove[i];
        if (!c) break;
        if (strchr(".x-=+#!?", c)) continue;
        if (strchr("oO0"     , c)) { castle++; continue; }
        if (c>='a' && c<='h') if (fromx<0) { fromx=tox; tox=c-'a'; continue; }
        if (c>='1' && c<='8') if (fromy<0) { fromy=toy; toy=c-'1'; continue; }

        //must be a piece
        const char *spiece = strchr(PieceChars,c&~0x20);
        if (!spiece) return 0;

        if (tox>=0 || toy>=0) promo = spiece-PieceChars;
        else                  piece = spiece-PieceChars;
    }

    assert(fromx<8 && fromy<8);
    assert(  tox<8 &&   toy<8);

    //minimal input is
    //a) at east two castle chars (e.g. o-o, O-O-O), or
    //b) at least a target square (e.g. Nf3, e4, e2e4), or
    //c) at least the source and target file (e.g. 'de' for dxe4)
    if (!(castle>=2 || (tox>=0 && toy>=0) || (tox>=0 && fromx>=0))) return 0;

    //omitting castling, piece and full from square implies a pawn move
    //(e.g. e4, dxe4, de; but not e.g. Ne4, O-O, e2e4, e1g1)
    if (!piece && !castle && !(fromx>=0 && fromy>=0)) piece=Pawn;

    //FRC: support rook capture by king for explicit castling e.g. Kxh1, e1h1
    const Side &side = _board.side();
    if (tox>=0 && toy>=0 && side.rooks.isset(sq64(tox, toy))) {
        //the king always starts between the rooks,
        //so we can find out whether to castle oo or ooo
        //by looking at the file of the 'captured' rook.
        castle = 2+(tox<x64(side.king));
        tox    = -1; //clear non-matching rook file
    }

    //explicit castling needs a target file for move disambiguation
    if (castle) tox = castle>2?C1:G1;


    //phase II: generate all moves, and look for a single matching move
    Move moves[MaxMoves];
    Move result = 0;
    Move *last = _board.genmoves(moves);
    for (Move *m=moves; m<last; m++) {
        Move move = *m;

        //pieces must match
        if (piece && piece!=_board.piece(move.from())) continue;
        if (move.isPromotion() && move.promotion()!=(promo?promo:Queen)) continue;

        //target square must match
        if (tox>=0 && x64(move.to())!=tox) continue;
        if (toy>=0 && y64(move.to())!=toy) continue;

        //source square must match
        if (fromx>=0 && x64(move.from())!=fromx) continue;
        if (fromy>=0 && y64(move.from())!=fromy) continue;

        //explicit castling must match
        if (castle && !move.isCastle()) continue;

        //implicit castling (e.g. e1g1, Kg1) is only allowed if the king will
        //not move exactly 1 square. In FRC, f1g1 could be a non castling move.
        if (!castle && move.isCastle() && (move.from()-1==move.to() ||
                                           move.from()+1==move.to())) continue;

        //we found a match
        //report an error in case there was already a different matching move
        if (result) {
            char movea[16], moveb[16];
            format(movea, result);
            format(moveb, move  );
            fprintf(stderr, "'%s' matches both %s _and_ %s!\n", smove, movea, moveb);
            return 0;
        }

        result = move;
    }

    return result;
}

//formats a move in short algebraic notation (SAN)
void MoveParser::format(char san[16], Move move) {

    int n=0;

    if (!move) {
        //format null move
        san[n++]='%';
        san[n++]=0;
        return;
    }

    if (move.isCastle()) {
        //format castling as either O-O or O-O-O
        san[n++]='O';
        san[n++]='-';
        san[n++]='O';
        if (!iskingside(move.to())) {
            san[n++]='-';
            san[n++]='O';
        }
        san[n++]=0;
        return;
    }

    //format as target file+rank,
    //with optional capture, promotion and/or disambiguation rank/file
    Move moves[MaxMoves];
    bool file=0,rank=0,filerank=0;
    int  piece = _board.piece(move.from());
    if (piece==Pawn) file |= move.isCapture();

    //generate all moves and see if there are any ambiguities to resolve
    Move *last = _board.genmoves(moves);
    for (Move *m=moves; m<last; m++) {
        Move tmp = *m;
        //same destination of an other piece of the same typee?
        if (tmp.to()!=move.to()) continue;
        if (_board.piece(tmp.from())!=piece) continue;
        if (move.from ()==tmp.from()) continue; //promotion

        //disambiguation needed
             if (x64(move.from())==x64(tmp.from())) rank=1;
        else if (y64(move.from())==y64(tmp.from())) file=1;
        else filerank=1;
    }

    //prefer file over rank disambiguation if either will do
    if (filerank && !file && !rank) file=1;

    //write result
    if (piece!=Pawn)        san[n++]= PieceChars[piece&7];
    if (file)               san[n++]='a'+x64(move.from());
    if (rank)               san[n++]='1'+y64(move.from());
    if (move.isCapture  ()) san[n++]='x';
                            san[n++]='a'+x64(move.to());
                            san[n++]='1'+y64(move.to());
    if (move.isPromotion()) san[n++]='=';
    if (move.isPromotion()) san[n++]=PieceChars[move.promotion()&7];
//  if (move.isCheck    ())_san[n++]='+';
    san[n++] = 0;
}
