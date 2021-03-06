#ifndef ANIMATIONS_H
#define ANIMATIONS_H

#include "chessAnimatable.h"
#include "irenderer.h"
#include <glm/glm.hpp>

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
		DrawChessPiece( ChessPiece* piece, const glm::vec2& from) : m_piece(piece), m_from(from) {}

        void animate( const float& t, AnimData* d, IGame* game );
      private:
        ChessPiece* m_piece;
		glm::vec2 m_from;
        
    }; // DrawChessPiece

    class DrawTime: public Anim
    {
      public:
        DrawTime( ScoreTime* st, int player ) { m_scoreTime = st; m_player = player; }

        void animate( const float& t, AnimData* d, IGame* game );

      private:
        int m_player;
        ScoreTime* m_scoreTime;
        

    }; // DrawScore

    class DrawWinner: public Anim
  {
    public:
      DrawWinner( int player, string name, string how ) 
      { 
        m_player = player; 
        m_name = name; 
        m_how = how;
      }

      void animate( const float& t, AnimData* d, IGame* game );

      float controlDuration() const
      { return 1; }

      float totalDuration() const
      { return 1; }


    private:
      string m_name;
      string m_how;
      int m_player;

  };

} // visualizer

#endif // ANIMATION_H
