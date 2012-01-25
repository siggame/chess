#include "chess.h"
#include "chessAnimatable.h"
#include "frame.h"
#include "animations.h"

#include <QDialog>
#include <cmath>
#include <QSignalMapper>
#include "networkloop.h"

namespace visualizer
{

  Chess::Chess() : BaseAI( 0 )
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
    // TODO: Cleanup This Code.  It's terrible
    m_ipAddress = ipAddress;
    QDialog *chooseGame = new QDialog();

    QGridLayout *layout = new QGridLayout();
    chooseGame->setLayout( layout );

    QSignalMapper *mapper = new QSignalMapper(this);

    QLabel* gameNumber = new QLabel( "Game Number: (Enter Nothing For New Game)" );
    m_gameNumber = new QLineEdit();
    QPushButton *spectate = new QPushButton( "Spectate" );
    QPushButton *play = new QPushButton( "Play As Human Client" );

    layout->addWidget( gameNumber, 0, 0, 1, 0 );
    layout->addWidget( m_gameNumber, 1, 0, 1, 0 );
    layout->addWidget( spectate, 2, 0 );
    layout->addWidget( play, 2, 1 );

    mapper->setMapping( spectate, 0 );
    mapper->setMapping( play, 1 );

    connect( spectate, SIGNAL( clicked() ), mapper, SLOT( map() ) );
    connect( spectate, SIGNAL( clicked() ), chooseGame, SLOT( close() ) );
    connect( play, SIGNAL( clicked() ), mapper, SLOT( map() ) );
    connect( play, SIGNAL( clicked() ), chooseGame, SLOT( close() ) );

    connect( mapper, SIGNAL( mapped( int ) ), this, SLOT( conn( int ) ) );

    chooseGame->exec();

  }

  void Chess::init()
  {
    cout << "INIT" << endl;
  }

  bool Chess::run()
  {
    cout << "RUNNING" << endl;
    Frame turn;
        
    SmartPointer<Board> board = new Board();
    board->addKeyFrame( new DrawBoard() );
    turn.addAnimatable( board );
      
    for( vector<client::Piece>::iterator p = pieces.begin(); p != pieces.end(); p++ )
    {
      SmartPointer<ChessPiece> piece = new ChessPiece();

      piece->x = p->file()-1;
      piece->y = p->rank()-1;
      piece->type = p->type();
      piece->owner = p->owner();

      piece->addKeyFrame( new DrawChessPiece( piece ) );
      turn.addAnimatable( piece );
    }

    addFrame( turn );
    timeManager->setNumTurns( turnNumber()+2);
    timeManager->play();
    
    // We'll want to wait for user input.
    bool input = false;
    while( !input )
    {
      inputMutex.lock();

      if( m_playerMoved )
      {
        input = true;
        m_playerMoved = false;
      }

      inputMutex.unlock();
    }


    // Once we get it, we'll create the same board, but with the moved piece. 

    return true;

  }

  void Chess::end()
  {
  }

  void Chess::setup()
  {
    
    renderer->setCamera( 0, 0, 9, 9 );
    renderer->setGridDimensions( 9, 9 );
    
    resourceManager->loadResourceFile( "./plugins/chess/textures.r" );

    animationEngine->registerGame( this, this );
 
  }

  void Chess::conn( int button )
  {
    m_playerMoved = m_spectating = m_player = false;
    cout << "Connecting to: " <<  m_ipAddress << " as " << button << endl;
    c = client::createConnection();

    if( button )
    {
      // we're a player
      m_player = true;
    }

    if( !client::serverConnect( c, m_ipAddress.c_str(), "19000" ) )
    {
      THROW( Exception, "Could Not Connect To Server" );
    }

    if( !client::serverLogin( c, username(), password() ) )
    {
      THROW( Exception, "Invalid Login Credentials" );
    }

    int gameNumber;

    if( m_gameNumber->text().size() )
    {
      gameNumber = QVariant( m_gameNumber->text() ).toInt(); 
      if( !client::joinGame( c, gameNumber ) )
      {
        THROW( Exception, "Error Joining Game!" );
      }
    }
    else
    {
      gameNumber = client::createGame( c );
    }

    cout << "Connected to game: " << gameNumber << endl;

    m_spectating = true;

    timeManager->setNumTurns( 0 );

    setup();

    NetworkLoop* n = new NetworkLoop( this, c );
    n->start();
    
    Frame turn;
        
    SmartPointer<Board> board = new Board();
    board->addKeyFrame( new DrawBoard() );
    turn.addAnimatable( board );

    addFrame( turn );
      

  
  } // Chess::conn() 

  void Chess::loadGamelog( std::string gamelog )
  {

    // BEGIN: Initial Setup
    cout << "Load Chess Gamelog" << endl;

    setup();
    m_player = false;
   
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
    
    load();
  } // Chess::loadGamelog()

  void Chess::preDraw()
  {
    const Input& input = gui->getInput();
    
    if( input.leftRelease )
    {
      if( m_player )
      {
        int x = floor( input.x - 0.5 );
        int y = floor( input.y - 0.5 );
        cout << x << ", " << y << endl;
        cout << "LEFT: " << input.leftRelease << ", RIGHT: " << input.rightRelease << endl;
      }
    }

    if( m_spectating )
    {
    }

  }

  void Chess::postDraw()
  {
  }
  
  void Chess::load()
  {
    SmartPointer<Board> board = new Board();

    timeManager->setNumTurns( m_game->states.size() );
    
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
            
            piece->addKeyFrame( new DrawChessPiece( piece ) );
            turn.addAnimatable( piece );
        }
        
        addFrame( turn );
    }
    
    timeManager->play();
  }
    
} // visualizer

Q_EXPORT_PLUGIN2( Chess, visualizer::Chess );
