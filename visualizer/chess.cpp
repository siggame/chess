#include "chess.h"
#include "chessAnimatable.h"
#include "frame.h"
#include "animations.h"
#include "version.h"

#include <QDialog>
#include <sstream>
#include <iomanip>
#include <cmath>
#include <QSignalMapper>
#include "networkloop.h"

using namespace std;

namespace visualizer
{

  Log errorLog( "chess.log" ); 

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

    gui->checkForUpdate( "Chess Update Available At:\n" \
      "ftp://r99acm.device.mst.edu:2121/", BUILD_NO, VERSION_FILE );

    options->loadOptionFile( "./plugins/chess/chess.xml", "chess" );

    return i;
  } // PluginInfo Chess::getPluginInfo()

  void Chess::spectate( std::string ipAddress )
  {
    // TODO: Cleanup This Code.  It's terrible
    m_ipAddress = ipAddress;
    promotion = 'Q';
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
    Bitboard::init();
  }

  map<char, int> Chess::populatePieces()
  {
    map<char, int> p;
    p['K'] = p['Q'] = 1;
    p['B'] = p['N'] = p['R'] = 2;
    p['P'] = 8;

    return p;

  }

  void Chess::addCurrentBoard()
  {
    Frame turn;
    SmartPointer<ChessBoard> board = new ChessBoard();
    board->addKeyFrame( new DrawBoard() );
    turn.addAnimatable( board );
    
    map< char, int > killed[2];
    killed[0] = populatePieces();
    killed[1] = populatePieces();

    for( vector<client::Piece>::iterator p = pieces.begin(); p != pieces.end(); p++ )
    {
      SmartPointer<ChessPiece> piece = new ChessPiece();
      
      piece->x = p->file()-1;
      piece->y = p->rank()-1;
      piece->type = p->type();
      piece->owner = p->owner();

      killed[piece->owner][piece->type]--;

      piece->addKeyFrame( new DrawChessPiece( piece ) );
      turn.addAnimatable( piece );

    }

    for( size_t i = 0; i < 2; i++ )
    {
      float x = 7.95;
      float y = 3.5+0.5*i;
      for( map< char, int >::iterator p = killed[i].begin(); p != killed[i].end(); p++ )
      {
        //cout << (char)p->first << ":" << p->second << endl;
        while( p->second-- )
        {
          SmartPointer<ChessPiece> piece = new ChessPiece();
          piece->x = x;
          x+=0.3;
          piece->y = y;
          if( x > 10 )
          {
            if( i )
            {
              y+=0.5;
            }
            else
            {
              y-=0.5;
            }
            x = 7.95;
          }
          piece->type = p->first;
          piece->owner = i;

          //cout << "(" << piece->x << ", " << piece->y << ")" << endl;
          //cout << (char)piece->type << ":" << piece->owner << endl;


          piece->addKeyFrame( new DrawDeadPiece( piece ) );
          turn.addAnimatable( piece );
          
        }

      }
    }
 
    addFrame( turn );
    timeManager->setNumTurns( size() );
    timeManager->play();   
  }

  bool Chess::run()
  {
    m_playerMoved = false;


    if( m_player )
    {
      addCurrentBoard();
      // We'll want to wait for user input.
      bool input = false;
      while( !input )
      {
        inputMutex.lock();

        if( m_playerMoved )
        {
          input = true;
        }

        inputMutex.unlock();
      }

      addCurrentBoard();
    } 
    else
    {
      addCurrentBoard();
    }

    // Once we get it, we'll create the same board, but with the moved piece. 
    return true;

  }

  void Chess::end()
  {
  }

  void Chess::setup()
  {
    
    renderer->setCamera( 0, 0, 8, 8);
    renderer->setGridDimensions( 8, 8 );
    
    resourceManager->loadResourceFile( "./plugins/chess/textures.r" );

    m_playerMoved = m_spectating = m_player = false;
    lastP.x = lastP.y = -1;
 
  }

  const char PieceNames[] =
  { 
    'R', 'N', 'B', 'Q', 'K', 'B', 'N', 'R',
    'P', 'P', 'P', 'P', 'P', 'P', 'P', 'P',
    'P', 'P', 'P', 'P', 'P', 'P', 'P', 'P',
    'R', 'N', 'B', 'Q', 'K', 'B', 'N', 'R',
  };

  void Chess::conn( int button )
  {
    setup();
    cout << "Connecting to: " <<  m_ipAddress << " as " << button << endl;
    c = client::createConnection();

    if( button )
    {
      // we're a player
      m_player = true;
    }

    if( !client::serverConnect( c, m_ipAddress.c_str(), "19000" ) )
    {
      WARNING( "Could Not Connect To Server" );
      return;
    }

    if( !client::serverLogin( c, username(), password() ) )
    {
      WARNING( "Invalid Login Credentials" );
      return;
    }

    int gameNumber;

    if( m_gameNumber->text().size() )
    {
      gameNumber = QVariant( m_gameNumber->text() ).toInt(); 
      if( m_player )
      {
        if( !client::joinGame( c, gameNumber, "player" ) )
        {
          WARNING( "Error Joining Game! See Console For Output" );
          return;
        }
      }
      else
      {
        if( !client::joinGame( c, gameNumber, "spectator" ) )
        {
          WARNING( "Error Joining Game!" );
          return;
        }
      }
    }
    else
    {
      gameNumber = client::createGame( c );
    }
    
    stringstream ss;
    ss << "Successfully connected to game: " << gameNumber << endl;

    QMessageBox::information( 0, "Game Connection", ss.str().c_str() );

    m_spectating = true;

    timeManager->setNumTurns( 0 );


    n = new NetworkLoop( this, c );
    n->start();
    
    Frame turn;
        
    SmartPointer<ChessBoard> board = new ChessBoard();
    board->addKeyFrame( new DrawBoard() );
    turn.addAnimatable( board );

    for( size_t i = 0; i < 32; i++ )
    {
      SmartPointer<ChessPiece> piece = new ChessPiece();
      
      piece->x = i-(i/8)*8;
      piece->y = i/8;
      piece->type = PieceNames[ i ];
      if( i < 16 )
      {
        piece->owner = 0;
      }
      else
      {
        piece->owner = 1;
        piece->y += 4;
      }
      
      piece->addKeyFrame( new DrawChessPiece( piece ) );
      turn.addAnimatable( piece );
    }

    addFrame( turn );

    animationEngine->registerGame( this, this );
  
  } // Chess::conn() 

  void Chess::loadGamelog( std::string gamelog )
  {

    // BEGIN: Initial Setup
    cout << "Load Chess Gamelog" << endl;

    setup();
    m_player = false;
   
    m_game = new parser::Game;

    if( !parser::parseGameFromString( *m_game, gamelog.c_str() ) )
    {
      delete m_game;
      m_game = 0;
      errorLog << gamelog;
      WARNING(
        "Cannot load gamelog, %s", 
        gamelog.c_str()
        );
    }
    // END: Initial Setup
    
    animationEngine->registerGame( this, this );

    load();
  } // Chess::loadGamelog()

  void Chess::preDraw()
  {
    const Input& input = gui->getInput();
    
    if( input.leftRelease )
    {
      if( m_player )
      {

        Coord p = getCoord();

        bool moved = false;
        if( lastP.x >= 0 && lastP.x < 8 && lastP.y >= 0 && lastP.y < 8 )
        {
          if( p.x >= 0 && p.x < 8 && p.y >= 0 && p.y < 8 )
          {
            for( vector<client::Piece>::iterator piece = pieces.begin(); piece != pieces.end() && !moved; piece++ )
            {
              if( piece->file() == lastP.x+1 && piece->rank() == lastP.y+1 )
              {
                if( moveIfValid( *piece, p.x, p.y, promotion ) )
                {
                  if( piece->type() == 'P' && ( p.y == 1 || p.y == 8 ) )
                  {
                    ((client::_Piece*)piece->ptr)->type = promotion;
                  }
                  inputMutex.lock();
                  m_playerMoved = true;
                  moved = true;
                  inputMutex.unlock();
                }
                lastP.x = lastP.y = -1;
              }

            }
          
          }
        }

        if( !moved )
        {
          lastP.x = p.x;
          lastP.y = p.y;
        }
      }
    }

    if( m_spectating )
    {
    }

  }

  Board Chess::buildBoardState() 
  {
    // Get valid moves for this board
    Board board;
    board.setstartpos();

    MoveParser parser( board );

    n->drawMutex.lock();
    for( vector<client::Move>::reverse_iterator m = moves.rbegin(); m != moves.rend(); m++ )
    {
      Move mv = parser.parse( printMove( *m, board ).c_str() );
      if( mv == 0 )
        cout << "Invalid Move!" << endl;
      board.move( mv );

    }
    n->drawMutex.unlock();

    return board;

  }

  bool Chess::moveIfValid( client::Piece& piece, int x, int y, int p )
  {
    if( m_playerMoved )
      return false;

    Board b = buildBoardState();
    Move moves[MaxMoves];
    Move *last = b.genmoves(moves);
    for( Move*m=moves; m<last; m++ )
    {
      if( (piece.file()-1 + 8*(piece.rank()-1)) == m->from() )
      {
        if( (x+8*y) == m->to() )
        {
          piece.move( x+1, y+1, p );
          for( vector<client::Piece>::iterator i = pieces.begin(); i != pieces.end(); i++ )
          {
            if( 
              i->file() == x+1 &&
              i->rank() == y+1 &&
              i->id() != piece.id() )
            {
              pieces.erase( i );
              break;
            }
          }

          return true;
        }
      }
    }
    
    return false;

  }

  void Chess::postDraw()
  {
    if( m_player )
    {
      
      stringstream player1;
      stringstream player2;

      if( players.size() )
      {
        int min = players[0].time()/60;
        int sec = players[0].time()-min*60;
        player2 << min << ":" << setw(2) << setfill( '0' ) << sec;
        min = players[1].time()/60;
        sec = players[1].time()-min*60;
        player1 << min << ":" << setw(2) << setfill( '0' ) << sec;
      } else
      {
        player1 << "WAITING";
        player2 << "WAITING";
      }

      bool rotate = options->getNumber( "RotateBoard" );
      renderer->setColor( Color( 0, 0, 0, 1 ) );
      renderer->drawProgressBar( promotion == 'Q' ? 10 : 11, 7, 1, 1, 1, Color( 0, 0.2, 0.9f, 0.7f ), 2, 1 );
      renderer->setColor( Color( 1, 1, 1 ) );

      if( playerID() )
      {
        renderer->drawText( 8, ( rotate ? 7.7 : 0 ), "DroidSansMono", player1.str(), 1 );
        renderer->drawText( 8, ( rotate ? 0 : 7.7 ), "DroidSansMono", player2.str(), 1 );

        renderer->drawTexturedQuad( 10, 7, 1, 1, "0-Q" );
        renderer->drawTexturedQuad( 11, 7, 1, 1, "0-N" );

      }
      else
      {
        renderer->drawText( 8, ( rotate ? 0 : 7.7 ), "DroidSansMono", player1.str(), 1 );
        renderer->drawText( 8, ( rotate ? 7.7 : 0 ), "DroidSansMono", player2.str(), 1 );

        renderer->drawTexturedQuad( 10, 7, 1, 1, "1-Q" );
        renderer->drawTexturedQuad( 11, 7, 1, 1, "1-N" );
      }

      renderer->drawText( 9.5, 6.7, "DroidSansMono", "Current Promotion:", 1 );

      
      if( lastP.y >= 0 && lastP.y < 8 )
      {
        if( lastP.x >= 0 && lastP.x < 8 )
        {
          // If the selector is in a valid position
          // Draw the box
          renderer->setColor( Color( 0, 0.2, 0.7, 0.3f ) );
          Coord select = lastP;
          select.y = 7-select.y;
          if( options->getNumber( "RotateBoard" ) )
          {
            select.y = 7-select.y;
            select.x = 7-select.x;
          }

          renderer->drawProgressBar( select.x, select.y, 1, 1, 1, Color( 0, 0.2, 0.9f, 0.7f ), 2, 1 );

          if( !m_playerMoved )
          {
            Board b = buildBoardState();
            Move moves[MaxMoves];
            Move* last = b.genmoves(moves);
            for( Move*m=moves;m<last;m++ )
            {
              if( (lastP.x+8*lastP.y) == m->from() )
              {
                int to = m->to();
                int y = to/8;
                int x = to-8*y;
                y = 7-y;

                if( options->getNumber( "RotateBoard" ) )
                {
                  x = 7-x;
                  y = 7-y;
                }

                renderer->setColor( Color( 0, 0.2, 0.7, 0.3f ) );
                renderer->drawProgressBar( x, y, 1, 1, 1, Color( 0.6, 0.2, 0.1, 0.7f ), 2 );

              }
            }
          }

        }
        else if( lastP.x < 0 && floor(lastP.y) == 7 )
        {
          if( lastP.x == -3 )
          {
            // Queen
            promotion = 'Q';
          }
          else if( lastP.x == -4 )
          {
            // Knight
            promotion = 'N';
          }
        }
      }
    }

  }
  
  void Chess::load()
  {
    SmartPointer<ChessBoard> board = new ChessBoard();

    timeManager->setNumTurns( m_game->states.size() );
    
    // Loop through each state in the gamelog
    for(int state = 0; state < m_game->states.size(); state++)
    {
        Frame turn;
        
        SmartPointer<ChessBoard> board = new ChessBoard();
        board->addKeyFrame( new DrawBoard() );
        turn.addAnimatable( board );
        
        // Loop though each Piece in the current state
        for(std::map<int, parser::Piece>::iterator i = m_game->states[ state ].pieces.begin(); i != m_game->states[ state ].pieces.end(); i++)
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

  string Chess::printMove( client::Move& m, Board &board ) 
  {

    string move;
    char piece = board.piecechar( m.fromFile()-1 + 8*(m.fromRank()-1) );
    move += piece;
    move += m.fromFile()+'a'-1;
    move += m.fromRank()+'0';
    move += m.toFile()+'a'-1;
    move += m.toRank()+'0';

    if( piece == 'P' )
    {
      if( m.toRank() == 1 || m.toRank() == 8 )
      {
        move += (char)m.promoteType();
      }
    }
  
    return move;
  }

  Coord Chess::getCoord() const
  {
    const Input& input = gui->getInput();
    Coord i;
    i.x = floor( input.x );
    i.y = 7-floor( input.y );

    if( options->getNumber( "RotateBoard" ) )
    {
      i.x = 7-i.x;
      i.y = 7-i.y;
    }

    return i;

  }
    
} // visualizer

Q_EXPORT_PLUGIN2( Chess, visualizer::Chess );
