#ifndef ANIMATIONS_H
#define ANIMATIONS_H

#include "chessAnimatable.h"
#include "irenderer.h"

namespace visualizer
{

    struct StartAnim: public Anim
    {
      public:
        void animate( const float& t, AnimData *d, IGame* game );

    };
  
    class DrawBoard: public Anim
    {
      public:
        void animate( const float& t, AnimData* d, IGame* game );

        float controlDuration() const
        { return 0; }

        float totalDuration() const
        { return 0; }

    }; // DrawBoard
  
    class DrawChessPiece: public Anim
    {
      public:
        DrawChessPiece( ChessPiece* piece ) { m_piece = piece; }

        void animate( const float& t, AnimData* d, IGame* game );
      private:
        ChessPiece* m_piece;
        
    }; // DrawChessPiece

}

#endif // ANIMATION_H
