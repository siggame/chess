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
  
    class DrawDeadPiece: public Anim
    {
      public:
        DrawDeadPiece( ChessPiece* piece ) { m_piece = piece; }

        void animate( const float& t, AnimData* d, IGame* game );
      private:
        ChessPiece* m_piece;
      
    };
  
    class DrawChessPiece: public Anim
    {
      public:
        DrawChessPiece( ChessPiece* piece ) { m_piece = piece; }

        void animate( const float& t, AnimData* d, IGame* game );
      private:
        ChessPiece* m_piece;
        
    }; // DrawChessPiece

    class DrawTime: public Anim
    {
      public:
        DrawTime( int time, int player ) { m_time = time; m_player = player; }

        void animate( const float& t, AnimData* d, IGame* game );

      private:
        int m_time;
        int m_player;
        

    }; // DrawScore

} // visualizer

#endif // ANIMATION_H
