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
    renderer->setUnitSize( 8, 8 );
    
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
        "Cannot load gamelog", 
        gamelog.c_str()
        );
    }
    
    SmartPointer<Board> board = new Board( renderer );
    // END: Initial Setup
    
    Frame turn;
    
    board->addKeyFrame( new DrawBoard( board ) );
    turn.addAnimatable( board );
    addFrame( turn );

    timeManager->setNumTurns( 1 );
    animationEngine->registerFrameContainer( this );
    timeManager->play();					//really don't need this beeteedubs

  } // Chess::loadGamelog()



} // visualizer

Q_EXPORT_PLUGIN2( Chess, visualizer::Chess );
