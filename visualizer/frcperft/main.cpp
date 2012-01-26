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
#include "common.h"
#include "Board.h"
#include "MoveParser.h"
#include "Timer.h"

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

//whether to perform 'bulk counting', ~10x faster than non-bulk mode.
//(non-bulk mode adds move+undo and pin- and attack calculation of all leaf moves)
#if !defined PERFT_FAST && !defined PERFT_BULK && !defined PERFT_SLOW
#define PERFT_FAST   1
#define PERFT_BULK  !PERFT_FAST
#define PERFT_SLOW   0
#endif

static Board      _board;
static MoveParser _parser(_board);

//minimal search time per position for self test
static double     _minseconds=1;

static const char *ExeName="frcperft";
static const char *Version="1.0";

#if PERFT_BULK
#define PERFT_NAME "BULK"
//perft with bulk option
//returns the number of moves for depth
static uint64 perft(int depth) {
    Move moves[MaxMoves];
    Move *last = _board.genmoves(moves);
    if (depth<=1) return last-moves;
    uint64 n=0;
    Undo undo = _board.save();
    for (Move *m=moves; m<last; m++) {
        _board.move(*m);
        n += perft(depth-1);
        _board.undo(*m, undo);
    }

    return n;
}
#endif

#if PERFT_FAST
#define PERFT_NAME "FAST"
//faster perft: like bulk, but does not generate (all) moves at the last ply
static uint64 perft(int depth) {
    Move moves[MaxMoves];
    if (depth<=1) return _board.countmoves(moves);

    Move *last = _board.genmoves(moves);
    uint64 n=0;
    Undo undo = _board.save();
    for (Move *m=moves; m<last; m++) {
        _board.move(*m);
        n += perft(depth-1);
        _board.undo(*m, undo);
    }

    return n;
}
#endif

#if PERFT_SLOW
#define PERFT_NAME "SLOW"
//slow non-bulk version: all moves are made and undone
static uint64 perft(int depth) {
    Move moves[MaxMoves];
    Move *last = _board.genmoves(moves);
    uint64 n=0;
    Undo undo = _board.save();
    for (Move *m=moves; m<last; m++) {
        _board.move(*m);
//      if (depth<2) n++; else n+=perft(depth-1);
        n+=(depth<2)?1:perft(depth-1);
        _board.undo(*m, undo);
    }

    return n;
}
#endif

//perft with timing & logging
static uint64 timeperft(int depth) {
    Timer timer;
    timer.start();
    uint64 n = perft(depth);
    timer.stop();
    double tickspo = timer.ticks()/double(n);
    double seconds = timer.seconds();
    double mnps    = n/(seconds*1000000); //meganodes/second
    printf("perft %i %14.0f %10.2fs %8.1f mnps %6.1f ticks/move\n", depth, double(n), seconds, mnps, tickspo);

    return n;
}

//perform perft with verified number of moves
void perft(const char *fen, ...) {
    _board.setfen(fen);
    _board.print();

    va_list va;
    va_start(va, fen);

    uint64 expected;
    int depth=0;
    Timer timer;
    timer.start();
    while ((expected=va_arg(va, uint64))) {
        depth++;
        uint64 actual = timeperft(depth);
        if (actual!=expected) {
            printf("Unexpected perf result, expected %0.0f, got %0.0f\n", double(expected), double(actual));
            abort();
        }
        timer.stop();
        if (timer.seconds()>_minseconds) {
            printf("break after %5.3f seconds\n", timer.seconds());
            break;
        }
    }
    va_end(va);
}

//unrolled perft: performs a perft(depth-1) for each move
//useful for debugging
static void divide(int depth) {
    Move moves[MaxMoves];
    Move *last = _board.genmoves(moves);
    char tmp[16];
    uint64 total=0;
    Undo undo = _board.save();
    MoveParser parser(_board);
    for (Move *m=moves; m<last; m++) {
        _board.move(*m);
        uint64 n = depth>1?perft(depth-1):1;
        total+= n;
        _board.undo(*m, undo);
        _parser.format(tmp, *m);
        printf("%-8s %10.0f\n", tmp, double(n));
    }
    printf("-------------------\n");
    printf("total    %10.0f\n", double(total));
}

//print all moves of the current position
static void logmoves() {
    char tmp[16];
    Move moves[MaxMoves];
    Move *last = _board.genmoves(moves);
    MoveParser parser(_board);
    for (Move *m=moves;m<last; m++) {
        _parser.format(tmp, *m);
        printf("%s ", tmp);
    }
    printf("\n%i moves\n", int(last-moves));
}

//verify some known perft values
static void verify() {
    //opening position
    perft("rnbqkbnr/pppppppp/////PPPPPPPP/RNBQKBNR w KQkq - -",
                  20ULL,
                 400ULL,
                8902ULL,
              197281ULL,
             4865609ULL,
           119060324ULL,
          3195901860ULL,0ULL);

    //promotions
    perft("8/PPP4k/8/8/8/8/4Kppp/8 w - - 0 1",
                  18ULL,
                 290ULL,
                5044ULL,
               89363ULL,
             1745545ULL,
            34336777ULL,
           749660761ULL,
         16303466487ULL,
        389736558441ULL,
       9169262376292ULL,0ULL);

    perft("8/7p/p5pb/4k3/P1pPn3/8/P5PP/1rB2RK1 b - d3 0 28",
                   5ULL,
                 117ULL,
                3293ULL,
               67197ULL,
             1881089ULL,
            38633283ULL,
          1069189070ULL,
         22488501780ULL,
        614975507497ULL,0ULL);

    perft("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 25",
                 48ULL,
               2039ULL,
              97862ULL,
            4085603ULL,
          193690690ULL,
         8031647685ULL,0ULL);

    perft("8/3K4/2p5/p2b2r1/5k2/8/8/1q6 b - - 1 67",
                 50ULL,
                279ULL,
              13310ULL,
              54703ULL,
            2538084ULL,
           10809689ULL,
          493407574ULL,0ULL); //ok!

    //ep
    perft("8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - 0 1",
                 14ULL,
                191ULL,
               2812ULL,
              43238ULL,
             674624ULL,
           11030083ULL,
          178633661ULL,
         3009794393ULL,
        50086749815ULL,
       860322602309ULL,
     14621463323674ULL,0ULL);

__frc__(
    //FRC testcases from
    //http://www.open-aurec.com/wbforum/viewtopic.php?t=1404
    perft("2rkr3/5PP1/8/5Q2/5q2/8/5pp1/2RKR3 w KQkq - 0 1",
                 51ull,
               1904ull,
              71005ull,
            2583102ull,
           94370149ull,
         3396258275ull,
       123548897773ull, 0ull);

    perft("bnrbnkrq/pppppppp/8/8/8/8/PPPPPPPP/BNRBNKRQ w KQkq - 0 1",
                 21ull,
                441ull,
              10179ull,
             233585ull,
            5863166ull,
          145999259ull,0ull);

    perft("rbknqnbr/pppppppp/8/8/8/8/PPPPPPPP/RBKNQNBR w KQkq - 0 1",
                 20ull,
                400ull,
               9032ull,
             203232ull,
            5089230ull,
          126480040ull,0ull);

    perft("qbrnnkbr/pppppppp/8/8/8/8/PPPPPPPP/QBRNNKBR w KQkq - 0 1",
                 20ull,
                400ull,
               9006ull,
             200996ull,
            4968926ull,
          121613156ull,0ull);

    perft("nqbnrbkr/pppppppp/8/8/8/8/PPPPPPPP/NQBNRBKR w KQkq - 0 1",
                 19ull,
                361ull,
               7801ull,
             166982ull,
            3978864ull,
           93888956ull,0ull);

    perft("nrnkbqrb/pppppppp/8/8/8/8/PPPPPPPP/NRNKBQRB w KQkq - 0 1",
                 19ull,
                361ull,
               7792ull,
             167475ull,
            4048343ull,
           97044730ull,0ull);
)
}

static void usage() {
    printf("\n");
    printf("Usage: %s [-divide] depth [\"fen\"]\n", ExeName);
    printf("or   : %s -verify [seconds]\n", ExeName);
    printf("or   : %s\n\n", ExeName);
}
//non-interactive mode: read and process command line arguments
static int doargs(int argc, char *argv[]) {
    bool optdivide=0;
    bool optverify=0;
    int  depth    =0;
    const char *fen=0;
    int arg=1;

    if (arg<argc && !strcmp("-divide", argv[arg])) { arg++; optdivide=true; }
    if (arg<argc && !strcmp("-verify", argv[arg])) { arg++; optverify=true; }
    if (arg<argc) { if (optverify) _minseconds=atof(argv[arg++]); else depth = atoi(argv[arg++]); }
    if (arg<argc) { fen = argv[arg++]; }

    if (arg<argc || !(depth || (optverify && !fen))) {
        usage();
        return 0;
    }

    if (fen && !_board.setfen(fen)) {
        printf("Illegal fen\n");
        return -1;
    }

    if (!optverify) _board.print();

         if (optverify) verify();
    else if (optdivide) divide(depth);
    else for (int i=1; i<=depth; i++) timeperft(i);

    return 0;
}

static void help() {
    usage();
    printf("\ninteractive commands\n");
    printf("--------------------------------------------------------------\n");
    printf("help              show this message\n");
    printf("fen xfen          setup a new (FRC) position specified by xfen\n");
    printf("perft depth       calculate perft for given depth\n");
    printf("divide depth      calculate perft for given depth,\n");
    printf("                  and show subtotals for each move\n");
    printf("moves             show all legal moves\n");
    printf("[piece][from]to   play a move, e.g. e4, Nf3, e2e4, O-O\n");
    printf("verify [seconds]  run a test of known perft results,\n");
    printf("                  spend at least [seconds] per position\n");
    printf("quit              exit\n");
}

//interactive mode: read and process user input
static int doinput() {
    _board.print();
    printf("interactive mode, type 'help'+Enter for help\n");

    while (1) {

        //read input
        printf("%% ");
        char buf[1024];
        char *cmd= fgets(buf, sizeof(buf), stdin);
        if (!cmd) break;

        int len = strlen(cmd);
        while (len && cmd[len-1]=='\n') cmd[--len]=0;

        //process input
        if (!len) {
            _board.print();
            continue;
        }

        strtok(cmd, " ");
        if (!strcmp("fen", cmd)) {
            const char*fen = strtok(0,"");
            if (fen) _board.setfen(fen);
            _board.print();
        }
        else if (!strcmp("quit", cmd)) break;
        else if (!strcmp("help", cmd)) help();
        else if (!strcmp("perft", cmd)) {
            const char*sdepth= strtok(0," ");
            int depth=1;
            if (sdepth) depth=atoi(sdepth);
            for (int i=1; i<=depth; i++) timeperft(i);
        }
        else if (!strcmp("divide", cmd)) {
            const char*sdepth= strtok(0," ");
            int depth=1;
            if (sdepth) depth=atoi(sdepth);
            if ( depth) divide(depth);
        }
        else if (!strcmp("m", cmd) || !strcmp("moves", cmd)) {
            logmoves();
        }
        else if (!strcmp("verify", cmd)) {
            const char*sseconds = strtok(0," ");
            if (sseconds) _minseconds = atof(sseconds);
            verify();
        }
        else {
            //if input looks like a move, try to play it
            bool ismove=1;
            for (int i=0; i<len; i++) if (!strchr("NBRQKabcdefgh12345678x-=+oO", cmd[i])) ismove=0;
            if (ismove) {
                Move move = _parser.parse(cmd);
                if (!move) {
                    printf("invalid move: '%s'\n", cmd);
                    continue;
                }
                _board.move(move);
                _board.print();
            }
            else {
                printf("unknown command: %s\n", cmd);
            }
        }
    }

    return 0;
}

//initialize and process command line parameters or user input
int main(int argc, char *argv[]) {
    printf("\n%s %s, (C) 2008-2011 AJ Siemelink\n", ExeName, Version);
    printf("single threaded, no hashing, mode=%s, extract=%s, count=%s\n",
           PERFT_NAME,
           EXTRACT_NAME,
           COUNT_NAME
           );
    Bitboard::init();
    _board.setstartpos();

    //arg-mode
    if (argc>=2) return doargs(argc, argv);

    //interactive mode
    return doinput();
}
