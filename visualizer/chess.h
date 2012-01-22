#ifndef CHESS_H
#define CHESS_H

#include <QObject>
#include "igame.h"
#include "animsequence.h"

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

  }; 

} // visualizer

#endif // CHESS_H
