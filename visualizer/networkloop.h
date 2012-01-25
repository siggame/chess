#ifndef NETWORKLOOP_H
#define NETWORKLOOP_H

#include <QThread>
#include "visclient/BaseAI.h"

namespace visualizer
{
  class NetworkLoop : public QThread
  {
    public:
      NetworkLoop( client::BaseAI* baseai, client::Connection* );

      void run();
      int exec() { return QThread::exec(); }

    private:
      client::BaseAI& ai;
      client::Connection* c;

  };
} // visualizer


#endif // NETWORKLOOP_H
