#ifndef NETWORKLOOP_H
#define NETWORKLOOP_H

#include <QThread>
#include <QMutex>
#include "visclient/BaseAI.h"

#ifdef _WIN32
#include <winsock2.h>
#pragma comment(lib, "Ws2_32.lib")
#else
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#define SOCKET_ERROR -1
#endif

#include <iostream>
using namespace std;

namespace visualizer
{
  class NetworkLoop : public QThread
  {
    public:
      NetworkLoop( client::BaseAI* baseai, client::Connection* );
      ~NetworkLoop()
      {
      }

      void run();
      int exec() { return QThread::exec(); }
      void kille()
      {
        m_suicide = true;
        destroyConnection( c );
        close( c->socket );
        while( isRunning() )
        {  }
        delete c;
        
      };

      bool suiciding()
      {
        return m_suicide;
      }

      QMutex drawMutex;

    private:
      client::BaseAI& ai;
      client::Connection* c;
      bool m_suicide;

  };
} // visualizer


#endif // NETWORKLOOP_H
