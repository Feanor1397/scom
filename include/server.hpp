#pragma once

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

  class Server
  {
    private:
      static void *serverRoutine(void* _args);
      pthread_t id;
    public:
      Server(const char* port);
      virtual ~Server();
      bool serverUp();
  };
}
