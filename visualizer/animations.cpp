#include "animations.h"
#include "chess.h"
#include <sstream>
#include <iomanip>

namespace visualizer
{
  void StartAnim::animate( const float& /* t */, AnimData * /* d */, IGame* game )
  {
  }

  void DrawWinner::animate( const float& t, AnimData* d, IGame* game )
  {
    game->renderer->setColor( Color( 1, 1, 1, t*.25 ) );
    game->renderer->drawQuad(0, 0, 8, 8);
    
    game->renderer->setColor( Color( 0, 0, 0, t ) );
    stringstream ss;
    ss << m_name << " Wins!!";
    game->renderer->drawText(2, 3, "LindenHill", ss.str(), 1.5 );
    if( m_player == -1 )
      game->renderer->drawText(2, 3.5, "LindenHill", "Stalemate", 1.5 );
    else
      game->renderer->drawText(2, 3.5, "LindenHill", m_how.c_str(), 1.5 );
    
    
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
        ss << (char)(game->options->getNumber( "RotateBoard" ) ? (104 - x) : (x + 97)) << (game->options->getNumber( "RotateBoard" ) ? (y + 1) : (8 - y));
		       
        // And raw the rank and file on the tile
        game->renderer->drawText(x, y, "DroidSansMono", ss.str(), 0.5f );

      }
    }
  }
   
  void DrawDeadPiece::animate( const float& /* t */, AnimData * /* d */, IGame* game )
  {
    ChessPiece &piece = *m_piece;
    
    stringstream ss;
    
    ss << 1-piece.owner << "-" << (char)piece.type;
    
    game->renderer->setColor( Color( 1, 1, 1 ) );
    if( game->options->getNumber( "RotateBoard" ) )
      game->renderer->drawTexturedQuad(piece.x, piece.y, 0.5, 0.5, ss.str());
    else
      game->renderer->drawTexturedQuad(piece.x, 7-piece.y, 0.5, 0.5, ss.str());

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

  void DrawTime::animate( const float& t, AnimData* d, IGame* game )
  {
    game->renderer->setColor( Color( 1, 1, 1 ) );

    stringstream ss;

    int min = m_scoreTime->time/60;
    int sec = m_scoreTime->time-min*60;

    ss << min << ":" << setw(2) << setfill( '0' ) << sec;

    bool rotate = game->options->getNumber( "RotateBoard" );

    if( 1-m_player )
    {
      game->renderer->drawText( 8, ( rotate ? 0.3 : 7.4 ), "DroidSansMono", ss.str(), 1 );
      game->renderer->drawText( 8, ( rotate ? 0.0 : 7.7 ), "LindenHill", m_scoreTime->playerName, 1 );
    }
    else
    {
      game->renderer->drawText( 8, ( rotate ? 7.4 : 0.3 ), "DroidSansMono", ss.str(), 1 );
      game->renderer->drawText( 8, ( rotate ? 7.7 : 0.0 ), "LindenHill", m_scoreTime->playerName, 1 );
    }

  }

}
