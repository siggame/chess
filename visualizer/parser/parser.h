#ifndef PARSER_H
#define PARSER_H

#include "structures.h"

namespace parser
{

bool parseFile(Game& game, const char* filename);
bool parseString(Game& game, const char* string);

} // parser

#endif
