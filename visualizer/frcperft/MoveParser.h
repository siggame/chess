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
#ifndef MOVE_PARSER_H_
#define MOVE_PARSER_H_

#include "Board.h"
#include "Move.h"

/*
 * Parses/formats moves from/to strings
 */
class MoveParser {
    Board &_board;

public:
    MoveParser(Board &board) : _board(board) {}

    //parses string to Move
    //returns 0 for invalid input
    Move parse(const char *move);

    //formats Move as SAN (short algebraic notation)
    void format(char sanmove[16], Move move);
}
;

#endif
