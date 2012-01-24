#include "chess.h"
#include "chessAnimatable.h"
#include "frame.h"
#include "animations.h"

#include <QDialog>

namespace visualizer
{

  Chess::Chess()
  {
  } // Chess::Chess()

  Chess::~Chess()
  {
  } // Chess::~Chess()

  PluginInfo Chess::getPluginInfo()
  {
    PluginInfo i;
    i.searchLength = 1000;
    i.gamelogRegexPattern = "Piece";
    i.returnFilename = false;
    i.spectateMode = true;
    i.pluginName = "Chess Plugin v2";

    options->loadOptionFile( "./plugins/chess/chess.xml", "chess" );

    return i;
  } // PluginInfo Chess::getPluginInfo()

  void Chess::spectate( std::string ipAddress )
  {
    m_ipAddress = ipAddress;
    QDialog *chooseGame = new QDialog();

    QGridLayout *layout = new QGridLayout();
    chooseGame->setLayout( layout );

    QLabel* gameNumber = new QLabel( "Game Number: (Enter Nothing For New Game)" );
    m_gameNumber = new QLineEdit();
    QPushButton *spectate = new QPushButton( "Spectate" );
    QPushButton *play = new QPushButton( "Play As Human Client" );

    layout->addWidget( gameNumber, 0, 0, 1, 0 );
    layout->addWidget( m_gameNumber, 1, 0, 1, 0 );
    layout->addWidget( spectate, 2, 0 );
    layout->addWidget( play, 2, 1 );

    connect( spectate, SIGNAL( clicked() ), this, SLOT( beSpectator() ) );
    connect( spectate, SIGNAL( clicked() ), chooseGame, SLOT( close() ) );
    connect( play, SIGNAL( clicked() ), this, SLOT( bePlayer() ) );
    connect( play, SIGNAL( clicked() ), chooseGame, SLOT( close() ) );

    chooseGame->exec();

  }

  void Chess::beSpectator()
  {
    cout << "Connecting to: " <<  m_ipAddress << "as spectator." << endl;
    cout << "GAME NUMBER: " << qPrintable( m_gameNumber->text() ) << endl;
  } // Chess::beSpectator() 

  void Chess::bePlayer()
  {
    cout << "Connecting to: " <<  m_ipAddress << "as player." << endl;
    cout << "GAME NUMBER: " << qPrintable( m_gameNumber->text() ) << endl;
  } // Chess::bePlayer()

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

  void Chess::preDraw()
  {
    const Input& input = gui->getInput();
    
    if( input.leftRelease || input.rightRelease )
    {
      cout << input.x << ", " << input.y << ", " << input.sx << ", " << input.sy << endl;
      cout << "LEFT: " << input.leftRelease << ", RIGHT: " << input.rightRelease << endl;
    }

  }

  void Chess::postDraw()
  {
  }
  
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
