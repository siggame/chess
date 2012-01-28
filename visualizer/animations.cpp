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
	Color black = Color( 0.1255, 0.125, 0.12 );
	Color white = Color( 0.875, 0.875, 0.88 );

    for(int x = 0; x < 8; x++)
    {
      for(int y = 0; y < 8; y++)
      {
        stringstream ss;
		
		// Set the tile's color to white or black based on it's location
		game->renderer->setColor( (x+y)%2 ? black : white  );
		
		// Draw the background of the tile
        game->renderer->drawQuad(x, y, 1, 1);
		
		// Set the color to gray for the rank & file text to be drawn in
		game->renderer->setColor( Color(0.8, 0.1, 0.1) );
		
		// Build the tile's rank & file string
		ss << (char)(x + 97) << (game->options->getNumber( "RotateBoard" ) ? (y + 1) : (8 - y));
		
		// And raw the rank and file on the tile
		game->renderer->drawText(x, y, "DroidSansMono", ss.str(), 1.0 );
      }
    }
  }
  
  void DrawChessPiece::animate( const float& /* t */, AnimData * /* d */, IGame* game )
  {
    ChessPiece &piece = *m_piece;
    
    stringstream ss;
    
    ss << 1-piece.owner << "-" << (char)piece.type;
    
    game->renderer->setColor( Color( 1, 1, 1 ) );
    if( game->options->getNumber( "RotateBoard" ) )
      game->renderer->drawTexturedQuad(7-piece.x, piece.y, 1, 1, ss.str());
    else
      game->renderer->drawTexturedQuad(piece.x, 7-piece.y, 1, 1, ss.str());

  }

}
