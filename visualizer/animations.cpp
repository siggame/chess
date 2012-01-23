#include "animations.h"

namespace visualizer
{
  void StartAnim::animate( const float& /* t */, AnimData * /* d */ )
  {
  }

  void DrawImage::animate( const float& /* t */, AnimData * /* d */ )
  {
    image &i = *m_image;
 
    i.renderer().setColor( Color( 1, 1, 1 ) );
    i.renderer().drawTexturedQuad( 0, 0, 40, 40, "visualExplosion" );

  }
  
  void DrawBoard::animate( const float& /* t */, AnimData * /* d */ )
  {
    Board &board = *m_board;
    
    for(int x = 0; x < 8; x++)
    {
      for(int y = 0; y < 8; y++)
      {
        board.renderer().setColor( Color (!((x + y ) % 2), !((x + y ) % 2), !((x + y ) % 2)) );
        board.renderer().drawQuad(x, y, 1, 1);
      }
    }
  }

}
