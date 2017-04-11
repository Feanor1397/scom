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
      NCursesPanel *appstd, *menu;
      scom::TextDuplex *textFields;
      pthread_t id;
      int uid;

      static void *clientRoutine(void* args);
      void send(const char* message);
      void send_im(int target_uid, const char* message);

    public:
      Client(const char* host, const char* port);
      virtual ~Client();
      void user_input();
      scom::ClientSocket* getSocket();
  };
}
