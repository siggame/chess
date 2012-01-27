#include "networkloop.h"

namespace visualizer
{

  NetworkLoop::NetworkLoop( client::BaseAI* baseai, client::Connection* conn ) : ai(*baseai), c(conn)
  {
  }

  void NetworkLoop::run()
  {
    while( client::networkLoop( c ) )
    {
      if( ai.startTurn() )
      {
        c->drawMutex.lock();
        client::endTurn(c);
        c->drawMutex.unlock();
      }
      else
      {
        c->drawMutex.lock();
        client::getStatus(c);
        c->drawMutex.unlock();
      }
    }
    ai.end();
    client::networkLoop(c);
    client::networkLoop(c);

  }

} // visualizer
