#include "animations.h"
#include "chess.h"
#include <sstream>

namespace visualizer
{
    void StartAnim::animate( const float& /* t */, AnimData * /* d */ )
    {
    }
    
    void DrawBoard::animate( const float& /* t */, AnimData * /* d */, IGame* game )
    {
        
        for(int x = 0; x < 8; x++)
        {
            for(int y = 0; y < 8; y++)
            {
                game->renderer->setColor( Color (!((x + y ) % 2), !((x + y ) % 2), !((x + y ) % 2)) );
                game->renderer->drawQuad(x, y, 1, 1);
            }
        }
    }
    
    void DrawChessPiece::animate( const float& /* t */, AnimData * /* d */, IGame* game )
    {
        ChessPiece &piece = *m_piece;
        
        stringstream ss;
        
        ss << piece.owner << "-" << (char)piece.type;
        
        game->renderer->setColor( Color( 1, 1, 1 ) );
        game->renderer->drawTexturedQuad(piece.x, piece.y, 1, 1, ss.str());
    }

}
