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
        client::endTurn(c);
      }
      else
      {
        client::getStatus(c);
      }
    }
    ai.end();
    client::networkLoop(c);
    client::networkLoop(c);

  }

} // visualizer
