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

    return lr;
  } // LogRegex Chess::logFileInfo()

  void Chess::loadGamelog( std::string toBeLalalaloaded )
  {
    cout << "Load Chess Gamelog" << endl;

    //resourceManager->loadTexture( toBeLalalaloaded, "visualExplosion" );
    Frame turn;
    //image *i = new image( renderer );
    //i->addKeyFrame( new StartAnim );
    //i->addKeyFrame( new DrawImage( i ) );
    //turn.addAnimatable( i );
    addFrame( turn );

    timeManager->setNumTurns( 1 );
    animationEngine->registerFrameContainer( this );
    timeManager->play();					//really don't need this beeteedubs

  } // Chess::loadGamelog()



} // visualizer

Q_EXPORT_PLUGIN2( Chess, visualizer::Chess );
