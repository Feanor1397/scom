#pragma once

#include <ui.hpp>
#include <client_socket.hpp>

#include <pthread.h>

namespace scom
{
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
      NCursesPanel *appstd, *menu;
      scom::TextDuplex *textFields;
    public:
      Client(const char* host, const char* port);
      virtual ~Client();
      void send();
      scom::ClientSocket* getSocket();
  };
}
