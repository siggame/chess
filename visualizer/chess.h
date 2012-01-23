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

        LogRegex logFileInfo();
        void loadGamelog( std::string gamelog );
        void run();
        
    private:
      Game *m_game;  // The Game Object from parser/structures.h that is generated by the Codegen

  }; 

} // visualizer

#endif // CHESS_H
