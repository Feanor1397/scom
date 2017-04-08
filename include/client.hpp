#pragma once

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#include <ui.hpp>

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
      int getFD();
  };

  struct ClientArgs
  {
    scom::ClientSocket *sock;
    scom::TextDuplex *ui;
  };

  class Client
  {
    private:
      scom::ClientSocket* socket;
      struct scom::ClientArgs args;
      static void *clientRoutine(void* args);
      pthread_t id;
    public:
      Client(const char* host, const char* port, scom::TextDuplex *ui);
      virtual ~Client();
      void send(const char* message);
      scom::ClientSocket* getSocket();
  };
}
