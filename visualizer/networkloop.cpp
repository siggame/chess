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
        drawMutex.lock();
        client::endTurn(c);
        drawMutex.unlock();
      }
      else
      {
        drawMutex.lock();
        client::getStatus(c);
        drawMutex.unlock();
      }
    }
    ai.end();
    client::networkLoop(c);
    client::networkLoop(c);

  }

} // visualizer
