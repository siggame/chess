FRC-perft 1.0, (c) 2008-2011 by AJ Siemelink


Introduction
-------------------------------------------------------------------
frcperft is a program that calculates the number of leaf
moves for a given chess or Fischer Random Chess position.

Technical features:
- full legal move generation
  using precalculated pins and enemy attacks
- bulk counting in leave nodes
  leave moves are counted, not made and undone on the board
- bitboards
- magic move generation by Pradu Kanan

Supported platforms are Windows 32/64, Mac OSX 32/64 and 
Linux 32/64 bit. Performance is best on 64-bit platforms 
with SSE4.2. On such a platform, frcperft spends less 
than 10 cpu cycles per move on the opening position. 


History
-------------------------------------------------------------------
The 2008 version of this program has been the starting point 
of my chess engine Spark.
The 2011 version (1.0), is an updated version that includes
performance improvements found in Spark and adds support for
Fischer Random Chess.


Running
-------------------------------------------------------------------
frcperft can be run in command line mode or in interactive mode.

Interactive mode is entered when frcperft is started without 
command line arguments. Use 'help' to see available commands.

For command line mode, use the following syntax:

	frcperft depth ["xfen"]

to calculate the perft number of a given xfen, if no xfen is 
supplied, the opening position for regular chess is used.


Compiling
-------------------------------------------------------------------
For g++, use the supplied makefile. 
Use 'make' with no arguments to see compilation options.

vc++ is not officially supported.
However, if you have the windows SDK installed, 
the makefile's 'vc' target might work for you, YMMV.


License
-------------------------------------------------------------------
This code is provided 'as-is', without any express or implied warranty.
In no event will the author be held liable for any damages arising from
the use of this code.

Permission is granted to anyone to use this code for non-commercial purposes.
This use includes compiling, altering and creating derivative work.

Redistribution of the code in source or binary form is allowed,
providing that the following conditions are met:

1. This notice may not be removed or altered from any source distribution.
   This notice must be included with any binary distribution.

2. The origin of this code must not be misrepresented;
   you must not claim that you wrote the original code.

3. Altered versions must be plainly marked as such, and must
   not be misrepresented as being the original code.

4. The distribution, including any accompanying works,
   must be provided free of charges and/or fees.

