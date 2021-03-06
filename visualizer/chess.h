#ifndef CHESS_H
#define CHESS_H

#include <QObject>
#include <QTime>
#include "igame.h"
#include "animsequence.h"

// The Codegen's Parser
#include "parser/parser.h"
#include "parser/structures.h"

#include "visclient/BaseAI.h"

#include "networkloop.h"
#include "frcperft/MoveParser.h"

#include <glm/glm.hpp>

namespace visualizer
{

  class Chess: public QObject, public AnimSequence, public IGame, public client::BaseAI
  {
	struct Coord
	{
	float x, y;
	};

    Q_OBJECT;
    Q_INTERFACES( visualizer::IGame );
    public: 
      Chess();
      ~Chess();

      PluginInfo getPluginInfo();
      void spectate( std::string ipAddress );
      void loadGamelog( std::string gamelog );

      void destroy();

      const char* username() { return options->getString( "Username" ).c_str(); }
      const char* password() { return options->getString( "Password" ).c_str(); }

      void load();
      void setup();

      void addCurrentBoard();

    public:
      void init();
      bool run();
      void end();

    public slots:
      void preDraw();
      void postDraw();
      void conn( int button );

    public:
      string printMove( client::Move& m, Board &board );
      Coord getCoord() const;

      Board buildBoardState();
      bool moveIfValid( client::Piece& piece, int x, int y, int p );

      map<char, int> populatePieces();

    private:
      parser::Game *m_game;  // The Game Object from parser/structures.h that is generated by the Codegen
      QLineEdit *m_gameNumber;
      std::string m_ipAddress;

      bool m_player;
      bool m_spectating;
    
      bool m_playerMoved;
      char promotion;
      QTime timeElapsed;
      int  m_whosTurn;

      map< char, int > wOffset, bOffset;
	  map< int, glm::vec2> m_LastPos;

      Coord lastP;

      QMutex inputMutex;
      QMutex m_suicideMutex;
      NetworkLoop* n;

      bool m_suicide;

	  void AddPieceToTurn(int id, SmartPointer<class ChessPiece>& piece, Frame& turn);

  }; 

} // visualizer

#endif // CHESS_H
