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

  };
} // visualizer


#endif // NETWORKLOOP_H
