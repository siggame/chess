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
#ifndef BITBOARD_H_
#define BITBOARD_H_

#include "common.h"
#include "chess.h"
#include "magicmoves.h"
#include <assert.h>

/*
 * bit extraction options.
 * can be set externally, e.g. 'g++ ... -DEXTRACT_DEBRUIJN=1'
 * the best choice depends on the target system.
 * - builtin  use gcc builtin, should generate bsfq on 64-bit systems
 * - bsf64    should be best, but requires a 64-bit system (cpu+os) with fast bsfq, e.g. intel corei
 * - bsf32    best suited for 32-bit systems with a fast 32-bit bsf
 * - debruijn best suited for any system with fast multiply and slow bsf, e.g. AMD-K8
 * - magic    best suited for 32-bit systems with slow multiply and slow bsf
 *
 * See also: http://chessprogramming.wikispaces.com/BitScan
 */
#if !defined EXTRACT_BUILTIN  && \
    !defined EXTRACT_BSF64    && \
    !defined EXTRACT_BSF32    && \
    !defined EXTRACT_DEBRUIJN && \
    !defined EXTRACT_MAGIC
#define EXTRACT_BUILTIN  (SYSTEM64 && defined __GNUC__)
#define EXTRACT_BSF64    (SYSTEM64 && !EXTRACT_BUILTIN)
#define EXTRACT_BSF32    (SYSTEM32 && !EXTRACT_BUILTIN)
#define EXTRACT_DEBRUIJN !(EXTRACT_BSF32 || EXTRACT_BSF64 || EXTRACT_BUILTIN)
#define EXTRACT_MAGIC    0
#endif

/*
 * bit counting options.
 * can be set externally, e.g. 'g++ ... -DCOUNT_LOOP=1'
 * the best choice depends on the target system.
 * - builtin  use gcc builtin, should generate popcntq on 64-bit systems with sse4.2
 * - popcnt   should be best, but requires a 64-bit system (cpu+os) with SSE4.2
 * - loop     uses a simple loop, not bad if a few bits are set
 * - wiki3    12 instructions with multiply from wikipedia
 * - wiki5    unrolled loop from wikipedia
 *
 * See also: http://en.wikipedia.org/wiki/Hamming_weight
 */
#if !defined COUNT_BUILTIN && \
    !defined COUNT_POPCNT  && \
    !defined COUNT_LOOP    && \
    !defined COUNT_WIKI3   && \
    !defined COUNT_WIKI5
#define COUNT_BUILTIN    (CPU_SSE42 && defined __GNUC__)
#define COUNT_POPCNT     (CPU_SSE42 && !COUNT_BUILTIN)
#define COUNT_LOOP       !(COUNT_POPCNT||COUNT_BUILTIN)
#define COUNT_WIKI3      0
#define COUNT_WIKI5      0
#endif


/*
 * bitmask lookup vs calculate option.
 * Whether to use precalculated cached masks for single squares.
 * leaving this option enabled appears to be a tad faster on 32-bit systems.
 */
#if !defined BIT_LOOKUP
#define BIT_LOOKUP   SYSTEM32
#endif


/*
 * Models a boolean value for each of the 64 squares of the chessboard.
 * Offers utility methods to extract / count bits and to generate piece attacks.
 *
 * 'magic moves' by Pradu Kanan are used to generate slider attacks.
 */
class Bitboard {

private:
    uint64 _data;

#if EXTRACT_MAGIC||EXTRACT_DEBRUIJN
    static align64 const uint Magic[];
#endif
#if BIT_LOOKUP
    static align64 uint64 Square[64];
#endif

    static align64 uint64 FileRank[2][64];
    static align64 uint64 Diagonal[2][64];
    static align64 uint64 Between[64][64]; //32Kb
    static align64 uint64 Line   [64][64]; //32Kb

    //mask for attacks of all pieces on an empty board
    static align64 uint64 Queen  [64];
    static align64 uint64 Bishop [64];
    static align64 uint64 Rook   [64];
    static align64 uint64 Knight [64];
    static align64 uint64 King   [64];
    static align64 uint64 Pawn[2][64]; //[color][sq]
    static align64 uint64 Side   [64]; //left&right

public:

#if BIT_LOOKUP
    inline static const uint64 &bit(int sq) {
        assert(sq<64);
        return Square[sq];
    }
#else
    inline static uint64 bit(int sq) {
        assert(sq<64);
        return 1ull<<sq;
    }
#endif

    static Bitboard file(int sq)   { return FileRank[0][sq]; }
    static Bitboard rank(int sq)   { return FileRank[1][sq]; }

    //NOTE: includes from nor to
    static Bitboard between(int from, int to) { return Between[from][to]; }
    static Bitboard line   (int from, int to) { return Line   [from][to]; }

    //piece attack masks (for an empty board)
    static Bitboard nmask (int sq) { return Knight[sq]; }
    static Bitboard kmask (int sq) { return King  [sq]; }
    static Bitboard rmask (int sq) { return Rook  [sq]; }
    static Bitboard qmask (int sq) { return Queen [sq]; }
    static Bitboard bmask (int sq) { return Bishop[sq]; }
    static Bitboard pmaskw(int sq) { return Pawn[White][sq]; }
    static Bitboard pmaskb(int sq) { return Pawn[Black][sq]; }
    static Bitboard pmask (int stm, int sq) { return Pawn[stm][sq]; }
    static Bitboard sidemask(int sq) { return Side[sq]; }

    Bitboard() {}
	Bitboard(uint64 data) { _data = data; }
    operator uint64 ()     const { return _data; }

    Bitboard operator^ (uint64 value) const { return _data^value; }
    Bitboard operator& (uint64 value) const { return _data&value; }
    Bitboard operator| (uint64 value) const { return _data|value; }
    Bitboard operator+ (uint64 value) const { return _data+value; }
    Bitboard operator- (uint64 value) const { return _data-value; }
    Bitboard operator>>(int n) const { return _data>>n; }
    Bitboard operator<<(int n) const { return _data<<n; }
    Bitboard operator~() const { return ~_data; }
    bool     operator!() const { return !_data; }

    Bitboard & operator =(uint64 value) { _data = value; return *this; }
    Bitboard & operator^=(uint64 value) { _data^= value; return *this; }
    Bitboard & operator&=(uint64 value) { _data&= value; return *this; }
    Bitboard & operator|=(uint64 value) { _data|= value; return *this; }
    Bitboard & operator+=(uint64 value) { _data+= value; return *this; }
    Bitboard & operator-=(uint64 value) { _data-= value; return *this; }
    Bitboard & operator>>=(int n){ _data>>=n; return *this; }
    Bitboard & operator<<=(int n){ _data<<=n; return *this; }

    //individual bit manipulation
    bool isset   (int sq) const { return _data & bit(sq); }
    void set     (int sq)       { _data |= bit(sq); }
    void flip    (int sq)       { _data ^= bit(sq); }
    void clear   (int sq)       { _data &=~bit(sq); }

    //clears the least significant bit
    void clearlsb() { _data &= _data-1; }

    //contains >1 bits?
    bool ismultiple() const { return _data&(_data-1); }
    //contains exactly one bit?
    bool issingular() const { return _data && !ismultiple(); }

    //piece attacks...
    Bitboard pattackswq() const { return (_data&~FileA)<<7; }
    Bitboard pattackswk() const { return (_data&~FileH)<<9; }
    Bitboard pattacksw () const { return pattackswq()|pattackswk(); }
    Bitboard pattacksbq() const { return (_data&~FileA)>>9; }
    Bitboard pattacksbk() const { return (_data&~FileH)>>7; }
    Bitboard pattacksb () const { return pattacksbq()|pattacksbk(); }
    Bitboard pattacks(int stm) const { return stm?pattacksb():pattacksw(); }
    Bitboard battacks(int  sq) const { return Bmagic(sq, _data); }
    Bitboard rattacks(int  sq) const { return Rmagic(sq, _data); }
    Bitboard qattacks(int  sq) const { return rattacks(sq)|battacks(sq); }

    //extracts a single bit
    int extract();

    //counts the number of set bits
    int count() const;

    void print(const char *desc) const ;

    //floodfills with king/knight moves
    Bitboard kflood() const;
    Bitboard nflood() const;

    //initializes static tables
	static void init();
};


#if     EXTRACT_BUILTIN
#define EXTRACT_NAME "BUILTIN"
    //inline assembly version for 64-bit systems
    inline int Bitboard::extract() {
        int result = __builtin_ctzll(_data);
        //clear least significant bit
        _data &= _data-1;
        return result;
    }
#endif

#if     EXTRACT_BSF64
#define EXTRACT_NAME "BSF64"
#include "asm.h"
    //inline assembly version for 64-bit systems
    inline int Bitboard::extract() {
        int result = _asm_::bsfq(_data);
        //clear least significant bit
        _data &= _data-1;
        return result;
    }
#endif

#if     EXTRACT_BSF32
#define EXTRACT_NAME "BSF32"
#include "asm.h"
    //inline assembly version for 32-bit systems
    inline int Bitboard::extract() {
        uint lo=_data;
        int result = lo?_asm_::bsf(lo):(32+_asm_::bsf(uint(_data>>32)));
        //clear least significant bit
        _data &= _data-1;
        return result;
    }
#endif

#if     EXTRACT_DEBRUIJN
#define EXTRACT_NAME "DEBRUIJN"
    //based on "Using de Bruijn Sequences to Index a 1 in a Computer Word"
    //by Leiserson, Prokop and Randall
    inline int Bitboard::extract() {
        uint64 bit = _data;
        //isolate least significant bit
        bit &= -bit;
        _data ^= bit;

        return Magic[((bit*0x07EDD5E59A4E28C2ULL) >> (64 - 6))];
    }
#endif

#if     EXTRACT_MAGIC
#define EXTRACT_NAME "MAGIC"
    //Matt Taylor's bit folding method
    inline int Bitboard::extract() {
        uint64 bit = _data;
        //isolate least significant bit
        bit &= -bit;
        _data ^= bit;
        bit--;
        return Magic[((uint(bit)^uint(bit>>32)) * 0x78291ACF) >> 26];
    }
#endif


#if     COUNT_BUILTIN
#define COUNT_NAME "BUILTIN"
    inline int Bitboard::count() const { return __builtin_popcountll(_data); }
#endif

#if     COUNT_POPCNT
#define COUNT_NAME "POPCNT"
#include "asm.h"
    inline int Bitboard::count() const { return _asm_::popcntq(_data); }
#endif

#if     COUNT_LOOP
#define COUNT_NAME "LOOP"
inline int Bitboard::count() const {
    uint64 x = _data;
    int n=0;
    if (x) do { n++; } while (x &= x-1);
    return n;
}
#endif

#if     COUNT_WIKI3
#define COUNT_NAME "WIKI3"
    //adapted from http://en.wikipedia.org/wiki/Hamming_weight
    //This uses fewer arithmetic operations than any other known
    //implementation on machines with fast multiplication.
    //It uses 12 arithmetic operations, one of which is a multiply.
    inline int Bitboard::count() const {
        static const uint64 m1  = 0x5555555555555555ull; //binary: 0101...
        static const uint64 m2  = 0x3333333333333333ull; //binary: 00110011..
        static const uint64 m4  = 0x0f0f0f0f0f0f0f0full; //binary:  4 zeros,  4 ones ...
        static const uint64 h01 = 0x0101010101010101ull; //the sum of 256 to the power of 0,1,2,3...

        uint64 x= _data;

        x -= (x >> 1) & m1;             //put count of each 2 bits into those 2 bits
        x = (x & m2) + ((x >> 2) & m2); //put count of each 4 bits into those 4 bits
        x = (x + (x >> 4)) & m4;        //put count of each 8 bits into those 8 bits
        return (x * h01)>>56;  //returns left 8 bits of x + (x<<8) + (x<<16) + (x<<24) + ...
    }
#endif

#if     COUNT_WIKI5
#define COUNT_NAME "WIKI5"
    //adapted from http://en.wikipedia.org/wiki/Hamming_weight
    //This is better if most bits in x are 0.
    //It uses 2 arithmetic operations and one comparison/branch  per "1" bit in x.
    //It is the same as the previous function, but with the loop unrolled.
    #define f(y) if ((x &= x-1) == 0) return y;
    inline int Bitboard::count() const {
        uint64 x=_data;
        if (x == 0) return 0;
        f( 1) f( 2) f( 3) f( 4) f( 5) f( 6) f( 7) f( 8)
        f( 9) f(10) f(11) f(12) f(13) f(14) f(15) f(16)
        f(17) f(18) f(19) f(20) f(21) f(22) f(23) f(24)
        f(25) f(26) f(27) f(28) f(29) f(30) f(31) f(32)
        f(33) f(34) f(35) f(36) f(37) f(38) f(39) f(40)
        f(41) f(42) f(43) f(44) f(45) f(46) f(47) f(48)
        f(49) f(50) f(51) f(52) f(53) f(54) f(55) f(56)
        f(57) f(58) f(59) f(60) f(61) f(62) f(63)
        return 64;
    }
#undef f
#endif

#endif
