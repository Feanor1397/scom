#pragma once

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

namespace scom
{
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
}
