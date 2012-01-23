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

  struct DrawImage: public Anim
  {
    DrawImage( image *i )
    {
      m_image = i;
    }

    void animate( const float& t, AnimData* d);
    
    float controlDuration() const
    {
      return 0;
    }

    float totalDuration() const
    {
      return 0;
    }

    private:
      image* m_image;

  };
  
  class DrawBoard: public Anim
  {
    public:
      DrawBoard( Board* board )
      {
        m_board = board;
      }

      void animate( const float& t, AnimData* d );

      float controlDuration() const
      { return 0; }

      float totalDuration() const
      { return 0; }

    private:
      Board* m_board;

  }; // DrawBoard

}

#endif // ANIMATION_H
