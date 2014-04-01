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
    game->renderer->drawText(4, 3, "LindenHill", ss.str(), 1.5, IRenderer::Center  );
#if 0
    if( m_player == -1 )
      game->renderer->drawText(4, 3.5, "LindenHill", "Stalemate", 0.6, IRenderer::Center );
    else
#endif
      game->renderer->drawText(4, 3.5, "LindenHill", m_how.c_str(), 0.6, IRenderer::Center );
    
    
  }
  
  void DrawBoard::animate( const float& /* t */, AnimData * /* d */, IGame* game )
  {
    unsigned int tileColor = (unsigned int)game->options->getNumber("Tile Color");

    Color whiteColorList[4] =
    {
      Color(0.875, 0.875, 0.88),
      Color(0.575, 0.575, 0.58),
      Color(0.2255, 0.225, 0.72),
      Color(0.1255, 0.525, 0.12)
    };

    Color black = Color( 0.1255, 0.125, 0.12 );

    for(int x = 0; x < 8; x++)
    {
      for(int y = 0; y < 8; y++)
      {
        stringstream ss;
		
        // Set the tile's color to white or black based on it's location
        game->renderer->setColor( (x+y)%2 ? black : whiteColorList[tileColor % 4] );
        
        // Draw the background of the tile
        game->renderer->drawQuad(x, y, 1, 1);
        
        // Set the color to gray for the rank & file text to be drawn in
        game->renderer->setColor( Color(0.8, 0.1, 0.1) );
		
        // Build the tile's rank & file string
        ss << (char)(game->options->getNumber( "Rotate Board" ) ? (104 - x) : (x + 97)) << (game->options->getNumber( "Rotate Board" ) ? (y + 1) : (8 - y));
		       
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
    if( game->options->getNumber( "Rotate Board" ) )
      game->renderer->drawTexturedQuad(piece.x, piece.y, 0.5, 0.5, ss.str());
    else
      game->renderer->drawTexturedQuad(piece.x, 7-piece.y, 0.5, 0.5, ss.str());

  }



  void DrawChessPiece::animate( const float& t, AnimData * /* d */, IGame* game )
  {
	const ChessPiece &piece = *m_piece;

	glm::vec2 diff = glm::vec2(piece.x,piece.y) - m_from;
	glm::vec2 pos = m_from + diff * t;
    
    stringstream ss;
	bool bRotateBoard = game->options->getNumber( "Rotate Board" ) > 0.0f;
    
    ss << 1-piece.owner << "-" << (char)piece.type;
    
    game->renderer->setColor( Color( 1, 1, 1 ) );
	if( bRotateBoard )
	  game->renderer->drawTexturedQuad(7-pos.x, pos.y, 1, 1, ss.str());
    else
	  game->renderer->drawTexturedQuad(pos.x, 7-pos.y, 1, 1, ss.str());

	if(game->options->getNumber( "Enable Move Arrows" ) > 0.0f)
	{
		if(diff != glm::vec2(0.0f))
		{
			float length = glm::length(diff);
			float angle = glm::degrees(atan2(diff.x, diff.y)) + 180.0f;

			game->renderer->push();

			if(bRotateBoard)
				game->renderer->translate((7.0f - m_from.x) + 0.5f, m_from.y + 0.5f);
			else
				game->renderer->translate(m_from.x + 0.5f, (7 - m_from.y) + 0.5f);

			game->renderer->rotate(angle,0.0f,0.0f,1.0f);
			game->renderer->translate(-0.5f,-0.5f);

			if(bRotateBoard)
				game->renderer->scale(1.0f, -length*1.1f);
			else
				game->renderer->scale(1.0f, length*1.1f);

			float yOffset = bRotateBoard ? -0.2f : 0.0f;

			// Draw the arrow
			game->renderer->setColor(Color( 1, 1, 1, 0.5f ));
			game->renderer->drawTexturedQuad(0.0f, yOffset, 1.0f, 1.0f, "move_arrow");
			game->renderer->pop();
		}
	}

  }

  void DrawTime::animate( const float& /*t*/, AnimData* /*d*/, IGame* game )
  {
    game->renderer->setColor( Color( 1, 1, 1 ) );

    stringstream ss;

    int min = m_scoreTime->time/60;
    int sec = m_scoreTime->time-min*60;

    ss << min << ":" << setw(2) << setfill( '0' ) << sec;

    bool rotate = game->options->getNumber( "Rotate Board" );

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
