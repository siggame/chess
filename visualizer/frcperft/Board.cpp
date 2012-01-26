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
#include "Board.h"

#include <string.h>
#include <stdlib.h>

//verify data structures after each move and undo?
//to enable: #define __verify__(x) x
#define __verify__(x)

#if !FRC
//castle info, indexed by [0=white,1=black][0=queenside,1=kingside]
//FRC: these are declared in Board.h, and calculated by initcastling()
static const align64 CastleInfo _castleinfo[2][2]={
{{0x000000000000000eull,0x000000000000001cull,E1,C1,A1,D1},
 {0x0000000000000060ull,0x0000000000000070ull,E1,G1,H1,F1}},
{{0x0e00000000000000ull,0x1c00000000000000ull,E8,C8,A8,D8},
 {0x6000000000000000ull,0x7000000000000000ull,E8,G8,H8,F8}}
};
//the castle rights that remain when a piece is moved from a square
static const uchar _castlemask[64]={
~CastleWq,~0,~0,~0,~CastleWkq,~0,~0,~CastleWk,
       ~0,~0,~0,~0,    ~0    ,~0,~0,~0,
       ~0,~0,~0,~0,    ~0    ,~0,~0,~0,
       ~0,~0,~0,~0,    ~0    ,~0,~0,~0,
       ~0,~0,~0,~0,    ~0    ,~0,~0,~0,
       ~0,~0,~0,~0,    ~0    ,~0,~0,~0,
       ~0,~0,~0,~0,    ~0    ,~0,~0,~0,
~CastleBq,~0,~0,~0,~CastleBkq,~0,~0,~CastleBk,
};
#endif

void Board::clear() {
// *this={}; not yet standard
    //beware: not ok if class has >0 virtual methods!
    memset(this, 0, sizeof(*this));
__frc__(
    memset(_castlemask, ~0, sizeof(_castlemask)));
}

void Board::print(FILE *file) const {
    if (!file) file=stdout;
    fprintf(file, "\n");

    const Side &white = _side[White];
    const Side &black = _side[Black];
    for (int yy=16; yy>=0; yy--) {
        int y = yy/2;
        if (!(yy&1)) {
            fprintf(file, "   +---+---+---+---+---+---+---+---+");
        }
        else {
            fprintf(file, " %i ", 1+y);
            for (int x=0; x<8; x++) {
                int sq = sq64(x,y);
                uchar c = _psq[sq];
                char piece = " pnbrqk"[c&7];
                     if (white.occupied.isset(sq)) fprintf(file, "|(%c)", piece^0x20);
                else if (black.occupied.isset(sq)) fprintf(file, "|*%c*", piece);
                else fprintf(file, "|   ");
            }
            fputs("|", file);
        }
        fputs("\n", file);
    }
    fprintf(file, "     a   b   c   d   e   f   g   h\n\n");
    char fenbuf[256];
    fen(fenbuf);
    fprintf(file, "%s\n", fenbuf);
}
__verify__(
void Board::fail(int stm, const char *desc, const char *desc2, Move move) {
    print();
    printf(" %i: %s: %s, m=%x, id=%p\n", stm, desc, desc2, (int)move, this);
    abort();
})
__verify__(
void Board::verify(int stm, const char *desc) {
    Side &side = _side[stm];

    if (side.occupied!=(side.pawns^
                        side.knights^
                        side.bishops^
                        side.rooks^
                        side.queens^
                        side.kings)) fail(stm, desc, "side occupied not ok\n");

    if (!side.kings.isset(side.king)) fail(stm, desc, "king not ok\n");
    if (_psq[side.king]!=King) fail(stm, desc, "king2 not ok\n");

    Bitboard froms = side.pawns;
    while (froms) {
        int from = froms.extract();
        if (_psq[from]!=Pawn) fail(stm, desc, "pawns not ok\n");
    }
    froms = side.knights;
    while (froms) {
        int from = froms.extract();
        if (_psq[from]!=Knight) fail(stm, desc, "N not ok\n");
    }
    froms = side.bishops;
    while (froms) {
        int from = froms.extract();
        if (_psq[from]!=Bishop) fail(stm, desc, "B not ok\n");
    }
    froms = side.rooks;
    while (froms) {
        int from = froms.extract();
        if (_psq[from]!=Rook) fail(stm, desc, "R not ok\n");
    }
    froms = side.queens;
    while (froms) {
        int from = froms.extract();
        if (_psq[from]!=Queen) fail(stm, desc, "Q not ok\n");
    }

    //todo: castling
})

__verify__(
void Board::verify(const char *desc, Move move) {
    verify(White, desc);
    verify(Black, desc);

    if (_occupied!=(_side[0].occupied|_side[1].occupied)) {
        fail(0, desc, "board occupied not ok", move);
    }
})


#if FRC
//FRC: calculates _castlemask and _castleinfo for stm
void Board::initcastling(int stm, int king, int qrook, int krook) {
    uchar *castling=_castlemask;

    castling[king ] ^= CastleWkq<<stm;
    castling[qrook] ^= CastleWq <<stm;
    castling[krook] ^= CastleWk <<stm;

    int sq=stm?A8:A1;
    CastleInfo  *info = _castleinfo[stm];

    //queen side
    info[0].kfrom = king;
    info[0].kto   = C1+sq; //sq64(C1,8*stm)
    info[0].rfrom = qrook;
    info[0].rto   = D1+sq;

    //king side
    info[1].kfrom = king;
    info[1].kto   = G1+sq;
    info[1].rfrom = krook;
    info[1].rto   = F1+sq;

    //calculate safe squares and required empty squares
    for (int i=0; i<2; i++) {
        int kfrom = info[i].kfrom;
        int kto   = info[i].kto;
        int rfrom = info[i].rfrom;
        int rto   = info[i].rto;
        //safe : all squares between king origin (inclusive) and destination (inclusive) must be safe from enemy attacks
        //empty: all squares between king origin (exclusive) and destination (inclusive)
        //                       and rook origin (exclusive) and destination (inclusive) must be empty,
        //                 EXCEPT the king origin and the rook origin (if passed over by rook or king)
        info[i].safe  = Bitboard::between(kfrom, kto)|BIT(kfrom)|BIT(kto);
        info[i].empty =(Bitboard::between(kfrom, kto)|BIT(kto)|
                        Bitboard::between(rfrom, rto)|BIT(rto)
                       ) & ~(BIT(kfrom)|BIT(rfrom));
    }
}
#endif

//FRC return the castling char unless the castling rook is not the outermost rook
//FRC in that case, return the file letter of the castling rook (HAha-style)
char Board::xfencastlechar(char c, int stm) const {
__frc__(
    const Side &side = _side[stm];
    bool iskingside  = (c|0x20)=='k';
    bool isqueenside = (c|0x20)=='q';
    Bitboard outer=0;
    int rook = _castleinfo[stm][iskingside].rfrom;
    if (iskingside ) outer = Bitboard::between(rook, rook| 7)|BIT(rook| 7);
    if (isqueenside) outer = Bitboard::between(rook, rook&~7)|BIT(rook&~7);
    outer &= ~BIT(rook);
    //iff any other rooks are lurking, replace the normal castle character
    //by the file letter of the rook used in castling
    if (outer&side.rooks) c = char('A'+x64(rook))|(c&0x20);
)
    return c;
}

void Board::fen(char fen[256]) const {
    char *result=fen;
    for (int y=7; y>=0; y--) {
        int skip=0;
        for (int x=0; x<8; x++) {
            int sq = sq64(x,y);
            if (!_psq[sq]) { skip++; continue; }
            if (skip) { *result++=char('0'+skip); skip=0; }
            char c = piecechar(sq);
            if (_side[Black].occupied.isset(sq)) c ^= 0x20;
            *result++=c;
        }
        //superfluous, yet fen standard wants us to claim the empty space here:
        if (skip) *result++=char('0'+skip);
        if (y) *result++='/';
    }

    //side to move
    *result++=' ';
    *result++="wb"[_stm&1];

    //castling
    *result++=' ';
    if (_info.castling) {
        //Use X-FEN: always KQkq unless >1 rooks on the castling side and castling with inner rook
        if (_info.castling&CastleWk) *result++=xfencastlechar('K', White);
        if (_info.castling&CastleWq) *result++=xfencastlechar('Q', White);
        if (_info.castling&CastleBk) *result++=xfencastlechar('k', Black);
        if (_info.castling&CastleBq) *result++=xfencastlechar('q', Black);
    }
    else {
        *result++='-';
    }

    //en passent
    *result++=(' ');
    if (_info.epsq) {
        *result++=char('a'+x64(_info.epsq));
        *result++=char('1'+y64(_info.epsq^8)); //^8: square behind the pawn
    }
    else *result++='-';

    //50 move counter and move number
    result += sprintf(result, " %i %i", fifty(), 1+_info.ply/2);
}


//
//identifies & validates the correct rook for the fen castling character.
//returns the corresponding castlemask
//
//FRC rules:
//1. king is always between rooks
//2. KQkq signify the outermost rook on the King/Queen side
//3. abcdefgh signify the rook to castle with
static int parsecastlechar(Board &board, char castlechar, int rooks[2][2]) {
    char c = castlechar;
    int stm = (c&0x20)?1:0;
    c |= 0x20;
    const Side &side = board.side(stm);

    int castling=0, rook=-1;
    if (c=='k') {
        //outermost rook on the king side
        for (int sq=side.king|7; sq>side.king; sq--) if (side.rooks.isset(sq)) {
            rook=sq;
            castling=CastleWk<<stm;
            break;
        }
    }
    else if (c=='q') {
        //outermost rook on the queen side
        for (int sq=side.king&070; sq<side.king; sq++) if (side.rooks.isset(sq)) {
            rook=sq;
            castling=CastleWq<<stm;
            break;
        }
    }
__frc__(
    else if (c>='a' && c<='h') {
        //rook on file
        int file = c-'a';
        int sq   = (side.king&070)+file;
        if (side.rooks.isset(sq)) {
            rook = sq;
            castling=(rook>side.king?CastleWk:CastleWq)<<stm;
        }
    })
    else {
        fprintf(stderr, "fen: illegal castling char: %c\n", castlechar);
        return 0;
    }

    if (!castling) {
        fprintf(stderr, "fen: invalid castling, no rook for: '%c'\n", castlechar);
        return 0;
    }

#if !FRC
    //normal chess: king and rook must be at default positions
    if (side.king!=(stm?E8:E1)) {
        fprintf(stderr,"fen: invalid castling for standard chess, king not at e1/e8\n");
        return 0;
    }
    if (rook>=0 && !(Bitboard(BIT(A1)|BIT(H1)|BIT(A8)|BIT(H8)).isset(rook))) {
        fprintf(stderr,"fen: invalid castling for standard chess, rook not at a1/h1/a8/h8 for: '%c'\n", castlechar);
        return 0;
    }
#endif

    //all ok
__frc__(
    //FRC: remember the correct rook
    if (rooks) rooks[stm][rook>side.king] = rook;
);

    return castling;
}

//setup: add a piece to the board
void Board::addpiece(int stm, int type, int sq) {
    assert(sq>=A1 && sq<=H8);

    //add piece to stm's army
    Side &side = _side[stm];
    if (type==King) side.king=sq;
    side.flip(type, sq);

    //add piece to board
    _psq [sq]=type;
    _occupied.set(sq);
}

//setup board according to fen
//returns false for (some clearly) illegal positions
bool Board::setfen(const char *fen) {
    clear();

    int x=0, y=7;
    while (*fen) {
        char c=*fen++;
        if (!c || c==' ') break;
        if (y<0 || x>7+(c=='/')) {
            fprintf(stderr,"fen: invalid file or rank: %c%c\n", 'a'+x, '1'+y);
            return false;
        }
        switch (c) {
            case '/': y--; x=0; break;
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8': x+=c-'0'; break;
            case 'P': addpiece(White, Pawn  , sq64(x++,y)); break;
            case 'N': addpiece(White, Knight, sq64(x++,y)); break;
            case 'B': addpiece(White, Bishop, sq64(x++,y)); break;
            case 'R': addpiece(White, Rook  , sq64(x++,y)); break;
            case 'Q': addpiece(White, Queen , sq64(x++,y)); break;
            case 'K': addpiece(White, King  , sq64(x++,y)); break;
            case 'p': addpiece(Black, Pawn  , sq64(x++,y)); break;
            case 'n': addpiece(Black, Knight, sq64(x++,y)); break;
            case 'b': addpiece(Black, Bishop, sq64(x++,y)); break;
            case 'r': addpiece(Black, Rook  , sq64(x++,y)); break;
            case 'q': addpiece(Black, Queen , sq64(x++,y)); break;
            case 'k': addpiece(Black, King  , sq64(x++,y)); break;
            default : fprintf(stderr,"fen: invalid input for %c%c: %c\n", char('a'+x), char('1'+y), c);
                      return false;
        }
    }

    //side to move
    while (*fen==' ') fen++;
    if (*fen) switch (*fen) {
        case 'w': _stm=White; fen++; break;
        case 'b': _stm=Black; fen++; break;
        default : fprintf(stderr, "fen: invalid stm: %c\n", *fen);
                  return false;
    }

    //verify kings
    const Side &white = side(White);
    const Side &black = side(Black);
    for (int stm=0; stm<2; stm++) if (!side(stm).kings.issingular()) {
        fprintf(stderr, "fen: invalid position: not exactly 1 king for side %i\n", stm);
        return false;
    }
    if (Bitboard::kmask(white.king)&black.kings) {
        fprintf(stderr, "fen: invalid position: kings touch!\n");
        return false;
    }
    if (genattacks(_stm).isset(enemy().king)) {
        fprintf(stderr, "fen: illegal position, side to move can capture king?!\n");
        return false;
    }

    //castling...
    while (*fen==' ') fen++;
    int rooks[2][2]={}; //[stm][iskingside]
    while (*fen) {
        char c=*fen++;
        if (c=='-') break;
        if (c==' ') break;
        _info.castling |= parsecastlechar(*this, c, rooks);
    }
__frc__(
    //FRC calculate new castlemasks and castleinfo
    //FRC to reflect the position of king and castling rooks
    initcastling(White, white.king, rooks[White][0], rooks[White][1]);
    initcastling(Black, black.king, rooks[Black][0], rooks[Black][1]);
)

    //en passant...
    while (*fen==' ') fen++;
    if (*fen) {
        char c=*fen++;
        if (*fen && c!='-') {
            //ignore supplied rank: use side to move to find ep square
            fen++;
            int ep = sq64(c-'a',_stm?3:4);

            //set ep if it is actually usable
            if (enemy().pawns.isset(ep) && Bitboard::sidemask(ep)&side().pawns) {
                _info.epsq = ep;
            }
        }
    }

    //fifty move counter
    while (*fen==' ') fen++;
    if (*fen=='-') fen++;
    else _info.fifty = -strtol(fen, (char **)&fen, 10);

    //move number
//  while (*fen==' ') fen++;
//  if (*fen=='-') fen++;
//  else nmove = strtol(fen, (char **)&fen, 10);

    //update pins and attacks
    updateinfo();

    __verify__(verify("setfen"));
    return true;
}

/*-----------------------------utils-------------------------------------*/
//all attacks by a side (see through opponent king)
Bitboard Board::genattacks(int stm) const {
    const Side &side = _side[stm];
    Bitboard occupied = _occupied ^ _side[stm^1].kings;
    uint64   attacks  = Bitboard::kmask(side.king);

    attacks |= side.pawns.pattacks(stm);

    Bitboard froms;
    froms = side.knights;             while (froms) attacks |= Bitboard::nmask  (froms.extract());
    froms = side.bishops|side.queens; while (froms) attacks |= occupied.battacks(froms.extract());
    froms = side.rooks  |side.queens; while (froms) attacks |= occupied.rattacks(froms.extract());

    return attacks;
}

//generates mask of ALL pieces pinned to stm's king, both sides!
Bitboard Board::genpinned(int stm) const {

    const Side &side  = _side[stm];
    const Side &enemy = _side[stm^1];
    uint64 pinned=0ULL;

    Bitboard attackers = enemy.rooksqueens()&Bitboard::rmask(side.king);
    while (attackers) {
        int rook = attackers.extract();
        Bitboard between = Bitboard::between(side.king,rook)&_occupied;
//      if (!(between&(between-1ull))) pinned |= between;
        if (!between.ismultiple()) pinned |= between;
    }

    attackers = enemy.bishopsqueens()&Bitboard::bmask(side.king);
    while (attackers) {
        int bishop = attackers.extract();
        Bitboard between = Bitboard::between(side.king,bishop)&_occupied;
//      if (!(between&(between-1ull))) pinned |= between;
        if (!between.ismultiple()) pinned |= between;
    }

    return pinned;
}

/*-----------------------------moves-------------------------------------*/
void Board::movespecial(Move move) {
    Side &side  = _side[_stm];

    int from = move.from();
    int to   = move.to  ();
    if (move.isEp()) {
        //move own pawn
        side.move(Pawn, from, to);
        _psq[from] = 0;
        _psq[to  ] = Pawn;

        //remove enemy pawn
        _psq [to^8] = 0;
        _side[_stm^1].flip(Pawn, to^8);

        _info.fifty=_info.ply;
    }
    else if (move.isCastle()) {

        //kill castle rights
        _info.castling &= _castlemask[from];

        //move king and rook
        side.king = to;
        side.move(King, from, to);
        const CastleInfo &info = _castleinfo[_stm][iskingside(to)];
        side.move(Rook, info.rfrom, info.rto);
        //FRC: watch out, source/targets may overlap, so clear before set
        _psq[      from] = 0;
        _psq[info.rfrom] = 0;
        _psq[      to  ] = King;
        _psq[info.rto  ] = Rook;
    }
    else {
        //move is a promotion
        assert(move.isPromotion());

        //capture?
        if (move.isCapture()) {
            _side[_stm^1].flip(move.capture(), to);
            _info.castling &= _castlemask[to];
        }

        //remove pawn
        side.flip(Pawn, from);
        _psq[from] = 0;

        //add piece
        side.flip(_psq[to]=move.promotion(), to);

        _info.fifty= _info.ply;
    }

    _occupied = _side[White].occupied|_side[Black].occupied;
    _stm ^= 1;
    updateinfo();
}
void Board::move(Move move) {
    __verify__(verify("movestart"));

    _info.ply++;
    _info.epsq=0;

    //special cases first
    if0 (move.isSpecial()) {
        movespecial(move);
        __verify__(verify("movespecial"));
        return;
    }

    //normal move
    Side &side  = _side[_stm];
    Side &enemy = _side[_stm^=1];
    int from = move.from();
    int to   = move.to  ();
    assert(to!=enemy.king);

    //clear castle rights if for rook and king moves
    _info.castling &= _castlemask[from];

    //capture?
    if (move.isCapture()) {
        //update castling for possible rook capure
        _info.castling &= _castlemask[to];
        _info.fifty     = _info.ply;
        enemy.flip(move.capture(), to);
    }

    //move the piece
    if (from==side.king) side.king = to;
    int piece = _psq[from];
    _psq[to  ]= piece;
    _psq[from]= 0;

    //and update hash for moving the piece
    side.move(piece, from, to);
    if (piece==Pawn) {
        //only set epsq if it is a double pawn advance and a relevant enemy  pawn is present
        if (!((from^to)&8) && (enemy.pawns&Bitboard::sidemask(to))) _info.epsq = to;
        _info.fifty=_info.ply;
    }

    _occupied = _side[White].occupied|_side[Black].occupied;
    updateinfo();

    __verify__(verify("move"));
}

void Board::undospecial(Move move) {
    Side &side = _side[_stm^=1];
    int from = move.from();
    int to   = move.to();

    if (move.isEp()) {
        //move pawn
        side.move(Pawn, to, from);
        _psq[from]=Pawn;
        _psq[to  ]=0;

        //restore enemy pawn
        _psq[to^8]=Pawn;
        _side[_stm^1].flip(Pawn, to^8);
    }
    else if (move.isCastle()) {
        const CastleInfo &info = _castleinfo[_stm][iskingside(to)];
        side.king = from;
        side.move(King, to, from);
        side.move(Rook, info.rto, info.rfrom);
        //clear both first, then put back both, since squares may overlap in FRC
        _psq[to        ]= 0;
        _psq[info.rto  ]= 0;
        _psq[from      ]= King;
        _psq[info.rfrom]= Rook;
    }
    else {
        //must be a promotion then
        assert(move.isPromotion());

        //add pawn and remove promotion piece
        side.flip(move.promotion(), to);
        side.flip(Pawn, from);
        _psq[from] = Pawn;

        //undo capture, if any
        _psq[to]=0;
        if (move.isCapture()) {
            _side[_stm^1].flip((_psq[to]=move.capture()), to);
        }
    }

    _occupied = _side[White].occupied|_side[Black].occupied;
}
void Board::undo(Move move, const Undo &undo) {
    assert(move);
    __verify__(verify("undostart"));

    _info = undo;
    if0 (move.isSpecial()) {
        undospecial(move);
        __verify__(verify("undospecial"));
        return;
    }

    int from=move.from();
    int to  =move.to  ();
    Side &side = _side[_stm^= 1];

    //move piece
    if (to==side.king) side.king = from;
    side.move(_psq[from]=_psq[to], to, from);
    _psq[to]=0;

    //replace captured enemy piece
    if (move.isCapture()) {
        _side[_stm^1].flip((_psq[to]=move.capture()), to);
    }

    _occupied = _side[White].occupied|_side[Black].occupied;
    __verify__(verify("undo", move));
}

/*----------------------------move generation--------------------------------*/
//All attackers (except KING!) of a square
Bitboard Board::genattackers(int stm, int sq) const {
    const Side &side  = _side[stm];
    Bitboard attackers;
//  attackers = Bitboard::kmask(     sq) & side.kings;
    attackers = Bitboard::nmask(     sq) & side.knights;
    attackers|= Bitboard::pmask(stm^1, sq) & side.pawns;
    attackers|= _occupied.battacks(sq) & side.bishopsqueens();
    attackers|= _occupied.rattacks(sq) & side.  rooksqueens();
    return attackers;
}

inline static void addpawnmove(Move *&moves, Move move) {
    if (ispromotionsq(move.to())) {
        *moves++ = move|Move::PromoQueen;
        *moves++ = move|Move::PromoKnight;
        *moves++ = move|Move::PromoBishop;
        *moves++ = move|Move::PromoRook;
    }
    else *moves++ = move;
}

//generate EP captures, caller must ensure that _info.epsq is set
Move *Board::genepcaptures(Move *moves) const {
    assert(_info.epsq);
    int from;
    int epsq = _info.epsq;
    int to   = epsq^8;
    const Side &side =_side[_stm  ];
    const Side &enemy=_side[_stm^1];

    //capture towards queenside
    if (x64(epsq) && side.pawns.isset(from=epsq-1)) {
        //EP may not put own king in check.
        //normal pin detection does not work here since the
        //removed enemy pawn is on a different square.
        //
        //   +---+---+---+---+---+---+
        // 5 |   |   |   |   |   |   |
        //   +---+---+---+---+---+---+
        // 4 |(R)|   |(P)|*p*|   |*k*|  1. c2c4+  ... (dxc3 is illegal)
        //   +---+---+---+---+---+---+
        // 3 |   |   |   |   |   |   |
        //   +---+---+---+---+---+---+
        // 2 |   |   |   |   |   |   |
        //   +---+---+---+---+---+---+
        // 1 |   |   |   |   |   |   |
        //   +---+---+---+---+---+---+
        //     a   b   c   d   e   f
        //detect this by clearing/moving the pawns in the occupied set
        //and looking for enemy rooks, bishops and queens attacking the king
        Bitboard occupied = _occupied ^ Bitboard::bit(from) ^ Bitboard::bit(epsq) ^ Bitboard::bit(to);
        if (!(occupied.rattacks(side.king)&enemy.  rooksqueens()) &&
            !(occupied.battacks(side.king)&enemy.bishopsqueens()))
            *moves++ = Move(from, to, _psq[epsq])|Move::Ep;
    }

    //capture towards kingside
    if (x64(from=epsq+1) && side.pawns.isset(from)) {
        Bitboard occupied = _occupied ^ Bitboard::bit(from) ^ Bitboard::bit(epsq) ^ Bitboard::bit(to);
        if (!(occupied.rattacks(side.king)&enemy.  rooksqueens()) &&
            !(occupied.battacks(side.king)&enemy.bishopsqueens()))
            *moves++ = Move(from, to, _psq[epsq])|Move::Ep;
    }

    return moves;
}

//generate legal evasions when in check
Move *Board::genevasions(Move *moves) const {
    const Side &side = _side[_stm];
    Bitboard froms,tos;
    int      from, to;

    //safe king moves
    tos = Bitboard::kmask(side.king) &~ (side.occupied|_info.attacked);
    while (tos) {
        to = tos.extract();
        *moves++ = Move(side.king, to, _psq[to]);
    }

    //one or two checking pieces?
    Bitboard attackers = genattackers(_stm^1, side.king);
    assert(attackers);
    int attacker = attackers.extract();
    //if double check: no captures or blocking moves are possible
    if (attackers) return moves;

    //there is a single enemy attacker that checks our king
    //see if we can capture it or move something that is not pinned in between...
    Bitboard targets = Bitboard::between(attacker, side.king);
    targets.set(attacker);
    assert(!(targets&side.occupied));

    Bitboard pinned = _info.pinned;
    froms = side.knights&~pinned;
    while (froms) {
        from = froms.extract();
        tos  = targets&Bitboard::nmask(from);
        while (tos) {
            to = tos.extract();
            *moves++ = Move(from, to, _psq[to]);
        }
    }
    froms = side.bishopsqueens()&~pinned;
    while (froms) {
        from = froms.extract();
        tos  = targets&_occupied.battacks(from);
        while (tos) {
            to = tos.extract();
            *moves++ = Move(from, to, _psq[to]);
        }
    }
    froms = side.rooksqueens()&~pinned;
    while (froms) {
        from = froms.extract();
        tos  = targets&_occupied.rattacks(from);
        while (tos) {
            to = tos.extract();
            *moves++ = Move(from, to, _psq[to]);
        }
    }

    //pawns
    froms    = side.pawns&~pinned;
    targets ^= Bitboard::bit(attacker);
    if (_stm) {
        //single pushes
        tos = (froms>>8) & targets;
        while (tos) {
            to = tos.extract();
            addpawnmove(moves, Move(to+8, to));
        }
        //double pushes
        tos = ((((froms>>8) & ~_occupied) >> 8) & targets) & Rank5;
        while (tos) {
            to = tos.extract();
            *moves++ = Move(to+16, to);
        }
        //captures
        froms &= Bitboard::pmaskw(attacker);
        while (froms) addpawnmove(moves, Move(froms.extract(), attacker, _psq[attacker]));
    }
    else {
        //single pushes
        tos = (froms<<8) & targets;
        while (tos) {
            to = tos.extract();
            addpawnmove(moves,Move(to-8, to));
        }
        //double pushes
        tos = ((((froms<<8) & ~_occupied) << 8) & targets) & Rank4;
        while (tos) {
            to = tos.extract();
            *moves++ = Move(to-16, to);
        }
        //captures
        froms &= Bitboard::pmaskb(attacker);
        while (froms) addpawnmove(moves, Move(froms.extract(), attacker, _psq[attacker]));
    }

#if 1
    //EP capture evasion
    //only when the enemy pawn delivers the check, we cannot block a discovered check this way
    if (_info.epsq && attacker==_info.epsq) moves = genepcaptures(moves);
#else
    //this situation is so rare that the idea below does not speed things up:
    //when evading check, it turns out that we can use the precalculated pins.
    //EP is allowed if it does not put own king in check, this can happen only with rook(queen) like this:
    //   +---+---+---+---+---+---+
    // 5 |   |   |   |*k*|   |   |
    //   +---+---+---+---+---+---+
    // 4 |   |   |(P)|*p*|   |   |  1. c2c4+  ... (dxc3 is illegal)
    //   +---+---+---+---+---+---+
    // 3 |   |xBx|   |(R)|   |   |  there can be no white bishop or queen at b3:
    //   +---+---+---+---+---+---+  *k* would already have been in check before c4+
    // 2 |   |   |   |   |   |   |
    //   +---+---+---+---+---+---+
    // 1 |   |   |   |   |   |   |
    //   +---+---+---+---+---+---+
    //     a   b   c   d   e
    if (_info.epsq && attacker==_info.epsq) {
        if (x64(attacker) && side.pawns.isset(from=attacker-1) && !pinned.isset(from)) *moves++ = Move(from, attacker^8, _psq[attacker])|Move::EP;
        if (x64(from=attacker+1) && side.pawns.isset(from)     && !pinned.isset(from)) *moves++ = Move(from, attacker^8, _psq[attacker])|Move::EP;
    }
#endif

    return moves;
}

//template for normal move generation
//if Generate is false, the generation runs in 'count' mode
template<bool Generate>
Move *Board::gennormalmoves(Move *moves) const {
    const Side &side = _side[_stm  ];
    const Side &enemy= _side[_stm^1];

    Move move;
    Bitboard froms,tos;
    int from,to;

    //moves by pinned pawns: they can only move on the file or diagonal from king to pinner
    const Bitboard pinned = _info.pinned;
    froms = side.pawns&pinned;
    while (froms) {
        from = froms.extract();
        static const int DY[2]={8,-8};
        int dy = DY[_stm];
//      int dy = _stm?-8:8;
//      int dy = 8-(_stm<<4);
        Bitboard line = Bitboard::line(side.king,from);
        if (line.isset(to=from+dy)) {
            //pawn push? it cannot be a promotion or there would be no pin
            if (!_psq[to]) {
                *moves++ = Move(from, to);
                if (ispromotionsq(from-dy) && !_psq[to+=dy]) *moves++ = Move(from, to);
            }
        }
        else {
            //capture?
            //the pinned pawn cannot be a rook pawn
            //therefore to-1 and to+1 are valid squares on the board
            assert((1ull<<to)&~(FileA|FileH));
            line&=enemy.occupied;
            if (line.isset(to-=1) ||
                line.isset(to+=2)) addpawnmove(moves, Move(from, to, _psq[to]));
        }
    }

    //en passant?
    if0 (_info.epsq) moves=genepcaptures(moves);

    //castling, pawn pushes and normal captures
    froms = side.pawns&~pinned;
    if (_stm) {

        //castling
        if (_info.castling&CastleBkq) {
            const CastleInfo *info = _castleinfo[Black];
            if (_info.castling&CastleBq) {
                if (!((_occupied&info->empty)|(_info.attacked&info->safe))) {
                    __frc__(if (!pinned.isset(info->rfrom)))
                    *moves++ = Move(side.king, info->kto)|Move::Castle;
                }
            }
            if (_info.castling&CastleBk) {
                info++;
                if (!((_occupied&info->empty)|(_info.attacked&info->safe))) {
                    __frc__(if (!pinned.isset(info->rfrom)))
                    *moves++ = Move(side.king, info->kto)|Move::Castle;
                }
            }
        }

        //pawn captures
        tos = froms.pattacksbq()&enemy.occupied;
        if (!Generate) moves+=4*(tos&Rank1).count()+(tos&~Rank1).count();
        if ( Generate) while (tos) {
            to = tos.extract();
            addpawnmove(moves, Move(to+9,to,_psq[to]));
        }
        tos = froms.pattacksbk()&enemy.occupied;
        if (!Generate) moves+=4*(tos&Rank1).count()+(tos&~Rank1).count();
        if ( Generate) while (tos) {
            to = tos.extract();
            addpawnmove(moves, Move(to+7,to,_psq[to]));
        }

        tos = (froms>>8)&~_occupied;
        //promotion pushes
        {
            Bitboard promos = tos&Rank1;
            tos ^= promos;
            if (!Generate) moves+=promos.count()<<2;
            if ( Generate) while (promos) {
                to = promos.extract();
                move = Move(to+8,to);
                *moves++ = move|Move::PromoQueen;
                *moves++ = move|Move::PromoKnight;
                *moves++ = move|Move::PromoBishop;
                *moves++ = move|Move::PromoRook;
            }
        }
        //double pushes
        {
            Bitboard doubles = ((tos&Rank6)>>8)&~_occupied;
            if (!Generate) moves+=doubles.count();
            if ( Generate) while (doubles) {
                to = doubles.extract();
                *moves++ = Move(to+16,to);
            }
        }
        //single pushes
        if (!Generate) moves+=tos.count();
        if (Generate) while (tos) {
            to = tos.extract();
            *moves++ = Move(to+8,to);
        }
    }
    else {
        //castling
        if (_info.castling&CastleWkq) {
            const CastleInfo *info = _castleinfo[White];
            if (_info.castling&CastleWq) {
                if (!((_occupied&info->empty)|(_info.attacked&info->safe))) {
                    __frc__(if (!pinned.isset(info->rfrom)))
                    *moves++ = Move(side.king, info->kto)|Move::Castle;
                }
            }
            if (_info.castling&CastleWk) {
                info++;
                if (!((_occupied&info->empty)|(_info.attacked&info->safe))) {
                    __frc__(if (!pinned.isset(info->rfrom)))
                    *moves++ = Move(side.king, info->kto)|Move::Castle;
                }
            }
        }

        //captures
        tos = froms.pattackswq()&enemy.occupied;
        if (!Generate) moves+=4*(tos&Rank8).count()+(tos&~Rank8).count();
        if ( Generate) while (tos) {
            to = tos.extract();
            addpawnmove(moves, Move(to-7,to,_psq[to]));
        }
        tos = froms.pattackswk()&enemy.occupied;
        if (!Generate) moves+=4*(tos&Rank8).count()+(tos&~Rank8).count();
        if ( Generate) while (tos) {
            to = tos.extract();
            addpawnmove(moves, Move(to-9,to,_psq[to]));
        }

        tos = (froms<<8)&~_occupied;
        //promotion pushes
        {
            Bitboard promos = tos&Rank8;
            tos ^= promos;
            if (!Generate) moves+=promos.count()<<2;
            if ( Generate) while (promos) {
                to = promos.extract();
                move = Move(to-8,to);
                *moves++ = move|Move::PromoQueen;
                *moves++ = move|Move::PromoKnight;
                *moves++ = move|Move::PromoBishop;
                *moves++ = move|Move::PromoRook;
            }
        }
        //double pushes
        {
            Bitboard doubles = ((tos&Rank3)<<8)&~_occupied;
            if (!Generate) moves+=doubles.count();
            if ( Generate) while (doubles) {
                to = doubles.extract();
                *moves++ = Move(to-16,to);
            }
        }
        //single pushes
        if (!Generate) moves+=tos.count();
        if ( Generate) while (tos) {
            to = tos.extract();
            *moves++ = Move(to-8,to);
        }
    }

    //knights
    froms = side.knights&~pinned;
    while (froms) {
        from = froms.extract();
        tos  = Bitboard::nmask(from)&~side.occupied;
        if (!Generate) moves+=tos.count();
        if ( Generate) {
            move = Move::create(from);
            while (tos) {
                to = tos.extract();
                *moves++ = move.gencapture(to, _psq[to]);
            }
        }
    }

    //bishops
    froms = side.bishopsqueens();
    while (froms) {
        from = froms.extract();
        tos  = _occupied.battacks(from)&~side.occupied;
        //only along pindir...
        if (pinned.isset(from)) tos &= Bitboard::line(side.king, from);
        if (!Generate) moves+=tos.count();
        if ( Generate) {
            move = Move::create(from);
            while (tos) {
                to = tos.extract();
                *moves++ = move.gencapture(to, _psq[to]);
            }
        }
    }

    //rooks
    froms = side.rooksqueens();
    while (froms) {
        from = froms.extract();
        tos  = _occupied.rattacks(from)&~side.occupied;
        //only along pindir...
        if (pinned.isset(from)) tos &= Bitboard::line(side.king, from);
        if (!Generate) moves+=tos.count();
        if ( Generate) {
            move = Move::create(from);
            while (tos) {
                to = tos.extract();
                *moves++ = move.gencapture(to, _psq[to]);
            }
        }
    }

    //king
    from = side.king;
    tos  = Bitboard::kmask(from)&~(side.occupied|_info.attacked);
    if (!Generate) moves+=tos.count();
    if ( Generate) {
        move = Move::create(from);
        while (tos) {
            to = tos.extract();
            *moves++ = move.gencapture(to, _psq[to]);
        }
    }

    return moves;
}

//instantiate templates
template Move *Board::gennormalmoves<false>(Move *moves) const;
template Move *Board::gennormalmoves<true> (Move *moves) const;
