#ifndef CHESS_ANIMATABLE_H
#define CHESS_ANIMATABLE_H

#include "chessAnimatable.h"
#include "irenderer.h"

namespace visualizer
{
    struct Board: public Animatable
    {
        Board( IRenderer *render ) : Animatable( render )
        {}

        AnimData* getData()
        { return 0; }
    };
  
    struct ChessPiece: public Animatable
    {
        ChessPiece( IRenderer *render ) : Animatable( render )
        {}
        
        int x;
        int y;
        int owner;
        int type;
        
        AnimData* getData()
        { return 0; }
    };

} // visualizer

#endif // CHESS_ANIMATABLE_H
