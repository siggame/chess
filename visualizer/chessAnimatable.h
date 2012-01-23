#ifndef CHESS_ANIMATABLE_H
#define CHESS_ANIMATABLE_H

#include "chessAnimatable.h"
#include "irenderer.h"

namespace visualizer
{
    struct Board: public Animatable
    {
    };
  
    struct ChessPiece: public Animatable
    {
        int x;
        int y;
        int owner;
        int type;
        
    };

} // visualizer

#endif // CHESS_ANIMATABLE_H
