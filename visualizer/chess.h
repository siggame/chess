#ifndef CHESS_H
#define CHESS_H

#include <QObject>
#include "igame.h"
#include "animsequence.h"

// The Codegen's Parser
#include "parser/parser.h"
#include "parser/structures.h"

namespace visualizer
{

  class Chess: public QObject, public AnimSequence, public IGame
  {
    Q_OBJECT;
    Q_INTERFACES( visualizer::IGame );
    public: 
        Chess();
        ~Chess();

        PluginInfo getPluginInfo();
        void spectate( std::string ipAddress );
        void loadGamelog( std::string gamelog );
        void run();

        void beSpectator();
        void bePlayer();
      public:
        QLineEdit *m_gameNumber;

        std::string m_ipAddress;
 
      public slots:
        void preDraw();
        void postDraw();

    private:
      Game *m_game;  // The Game Object from parser/structures.h that is generated by the Codegen

  }; 

} // visualizer

#endif // CHESS_H
