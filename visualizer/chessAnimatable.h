#ifndef CHESS_ANIMATABLE_H
#define CHESS_ANIMATABLE_H

#include "chessAnimatable.h"
#include "irenderer.h"

namespace visualizer
{
  struct image: public Animatable
  {
    image( IRenderer *render ) : Animatable( render )
    {}

    AnimData* getData()
    {
      return 0;
    }

  };

} // visualizer

#endif // CHESS_ANIMATABLE_H
