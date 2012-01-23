#ifndef ANIMATIONS_H
#define ANIMATIONS_H

#include "chessAnimatable.h"
#include "irenderer.h"

namespace visualizer
{

    struct StartAnim: public Anim
    {
        public:
            void animate( const float& t, AnimData *d );

            float controlDuration() const
            {
                return 0;
            }

            float totalDuration() const
            {
                return 0;
            }
    };
  
    class DrawBoard: public Anim
    {
        public:
            DrawBoard( Board* board )
            {
                m_board = board;
            }

    void animate( const float& t, AnimData* d, IGame* game );

    float controlDuration() const
    { return 0; }

    float totalDuration() const
    { return 0; }

    private:
        Board* m_board;

    }; // DrawBoard
  
    class DrawChessPiece: public Anim
    {
        public:
            DrawChessPiece( ChessPiece* piece ) { m_piece = piece; }

            void animate( const float& t, AnimData* d, IGame* game );

            float controlDuration() const
            { return 0; }

            float totalDuration() const
            { return 0; }

    private:
        ChessPiece* m_piece;
        
    }; // DrawChessPiece

}

#endif // ANIMATION_H
