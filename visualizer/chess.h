#ifndef CHESS_H
#define CHESS_H

#include <QObject>
#include "igame.h"
#include "animsequence.h"

// The Codegen's Parser
#include "parser/parser.h"
#include "parser/structures.h"

#include "visclient/BaseAI.h"

#include "networkloop.h"

namespace visualizer
{

  class Chess: public QObject, public AnimSequence, public IGame, public client::BaseAI
  {
    Q_OBJECT;
    Q_INTERFACES( visualizer::IGame );
    public: 
      Chess();
      ~Chess();

      PluginInfo getPluginInfo();
      void spectate( std::string ipAddress );
      void loadGamelog( std::string gamelog );

      const char* username() { return options->getString( "Username" ).c_str(); }
      const char* password() { return options->getString( "Password" ).c_str(); }

      void load();
      void setup();

    public:
      void init();
      bool run();
      void end();

    public slots:
      void preDraw();
      void postDraw();
      void conn( int button );

    private:
      Game *m_game;  // The Game Object from parser/structures.h that is generated by the Codegen
      QLineEdit *m_gameNumber;
      std::string m_ipAddress;

      bool m_player;
      bool m_spectating;
    
      bool m_playerMoved;

      QMutex inputMutex;

  }; 

} // visualizer

#endif // CHESS_H
