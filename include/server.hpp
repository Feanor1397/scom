#pragma once

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#include <ui.hpp>

#include <pthread.h>

namespace scom
{
  static bool serverLock = false;

  struct ServArgs
  {
    const char* host;
    const char* port;
  };

  class ServerSocket
  {
    private:
      struct addrinfo hints, *ai;
      int i32SocketFD;
      int status;
      char buff[256];
    protected:
    public:
      ServerSocket(const char* port);
      virtual ~ServerSocket();
      int acceptConnection();
      int listenerFD();
      void send(int connection ,const char* message);
      const char* recv(int connection);
      void disconnect(int connection);
  };

  class Server
  {
    private:
      struct scom::ServArgs args;
      static void *serverRoutine(void* _args);
      pthread_t id;
    public:
      Server(const char* host, const char* port);
      virtual ~Server();
      bool serverUp();
  };
}
