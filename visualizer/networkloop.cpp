#include "networkloop.h"
#include <iostream>
using namespace std;

namespace visualizer
{

  NetworkLoop::NetworkLoop( client::BaseAI* baseai, client::Connection* conn ) : ai(*baseai), c(conn)
  {
    m_suicide = false;
  }

  void NetworkLoop::run()
  {
    while( client::networkLoop( c ) && !m_suicide )
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

    if( m_suicide )
    {
      return;
    }
    m_suicide = true;

    ai.end();
    client::networkLoop(c);
    client::networkLoop(c);

  }

} // visualizer
