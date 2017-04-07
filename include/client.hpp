#ifndef SCOM_CLIENT_HPP
#define SCOM_CLIENT_HPP

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

namespace scom
{
  class ClientSocket
  {
    private:
      struct addrinfo hints, *ai, *p;
      int i32SocketFD;
      int status;
      char buff[256];
    protected:
    public:
      ClientSocket(const char* host, const char* port);
      virtual ~ClientSocket();
      void connect();
      void disconnect();
      void send(const char* message);
      char const* recv();
  };
}

#endif
