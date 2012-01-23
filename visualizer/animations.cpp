#include "animations.h"
#include <sstream>

namespace visualizer
{
    void StartAnim::animate( const float& /* t */, AnimData * /* d */ )
    {
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
    
    void DrawChessPiece::animate( const float& /* t */, AnimData * /* d */ )
    {
        ChessPiece &piece = *m_piece;
        
        stringstream ss;
        
        ss << piece.owner << "-" << (char)piece.type;
        
        piece.renderer().setColor( Color( 1, 1, 1 ) );
        piece.renderer().drawTexturedQuad(piece.x, piece.y, 1, 1, ss.str());
    }

}
