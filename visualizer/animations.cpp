#include "animations.h"
#include "chess.h"
#include <sstream>

namespace visualizer
{
  void StartAnim::animate( const float& /* t */, AnimData * /* d */, IGame* game )
  {
  }
  
  void DrawBoard::animate( const float& /* t */, AnimData * /* d */, IGame* game )
  {
    game->renderer->translate( 0.5f, 0.5f );
    for(int x = 0; x < 8; x++)
    {
      for(int y = 0; y < 8; y++)
      {
        stringstream ss;
		
        //game->renderer->setColor( Color (!((x + y ) % 2), !((x + y ) % 2), !((x + y ) % 2)) );
        if( (x+y)%2 )
        {
          game->renderer->setColor( Color( 0.05, 0.05, 0.05 ) );
        }
        else
        {
          game->renderer->setColor( Color( 85.0f/255, 107.0f/255, 47.0f/255 ) );
        }
        game->renderer->drawQuad(x, y, 1, 1);
		
		game->renderer->setColor( Color(0.5, 0.5, 0.5) );
		
		ss << (char)(x + 97) << (8 - y);
		
		game->renderer->drawText(x, y, "DroidSansMono", ss.str(), 1.0 );
      }
    }
  }
  
  void DrawChessPiece::animate( const float& /* t */, AnimData * /* d */, IGame* game )
  {
    ChessPiece &piece = *m_piece;
    
    stringstream ss;
    
    ss << piece.owner << "-" << (char)piece.type;
    
    game->renderer->setColor( Color( 1, 1, 1 ) );
    if( game->options->getNumber( "RotateBoard" ) )
      game->renderer->drawTexturedQuad(7-piece.x, 7-piece.y, 1, 1, ss.str());
    else
      game->renderer->drawTexturedQuad(piece.x, piece.y, 1, 1, ss.str());

  }

}
