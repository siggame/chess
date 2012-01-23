#include "chess.h"
#include "chessAnimatable.h"
#include "frame.h"
#include "animations.h"

namespace visualizer
{

  Chess::Chess()
  {
  } // Chess::Chess()

  Chess::~Chess()
  {
  } // Chess::~Chess()

  LogRegex Chess::logFileInfo()
  {
    LogRegex lr;
    lr.regex = "Piece";
    lr.startSize = 1000;
    lr.giveFilename = false;

    return lr;
  } // LogRegex Chess::logFileInfo()

  void Chess::loadGamelog( std::string gamelog )
  {
    // BEGIN: Initial Setup
    cout << "Load Chess Gamelog" << endl;
    
    renderer->setCamera( 0, 0, 8, 8 );
    renderer->setGridDimensions( 8, 8 );
    
    resourceManager->loadResourceFile( "./plugins/chess/textures.r" );
    
    m_game = new Game;

    if( !parseString( *m_game, gamelog.c_str() ) )
    {
      delete m_game;
      m_game = 0;
      errorLog << gamelog;
      THROW
        (
        GameException,
        "Cannot load gamelog, %s", 
        gamelog.c_str()
        );
    }
    // END: Initial Setup
    
    run();
  } // Chess::loadGamelog()
    
    void Chess::run()
    {
        SmartPointer<Board> board = new Board();

        timeManager->setNumTurns( m_game->states.size() );
        animationEngine->registerGame( this, this );
        
        // Loop through each state in the gamelog
        for(int state = 0; state < m_game->states.size(); state++)
        {
            Frame turn;
            
            SmartPointer<Board> board = new Board();
            board->addKeyFrame( new DrawBoard() );
            turn.addAnimatable( board );
            
            // Loop though each Piece in the current state
            for(std::map<int, Piece>::iterator i = m_game->states[ state ].pieces.begin(); i != m_game->states[ state ].pieces.end(); i++)
            {
                SmartPointer<ChessPiece> piece = new ChessPiece();
                
                piece->x = i->second.file - 1;
                piece->y = i->second.rank - 1;
                piece->type = i->second.type;
                piece->owner = i->second.owner;
                
                board->addKeyFrame( new DrawChessPiece( piece ) );
                turn.addAnimatable( piece );
            }
            
            addFrame( turn );
        }
        
        timeManager->play();
    }
    
} // visualizer

Q_EXPORT_PLUGIN2( Chess, visualizer::Chess );
