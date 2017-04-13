#pragma once

#include <pthread.h>
#include <vector>
#include <protocol.hpp>

namespace scom
{
  static bool serverLock = false;

  class Server
  {
    struct userlist_s
    {
      int fileDescriptor;
      int uid;
      const char* name;
    };

    struct ServArgs
    {
      const char* port;
      std::vector<struct scom::Server::userlist_s>* userlist;
    };

    private:
      pthread_t id;
      std::vector<struct scom::Server::userlist_s> userlist;

      static void *serverRoutine(void* _args);
      /* methods to call inside thread */
      static int new_user(const char* name,
          std::vector<struct scom::Server::userlist_s>* userlist);
      static int find_free_place_for_user(
          std::vector<struct scom::Server::userlist_s>* userlist);
    public:
      Server(const char* port);
      virtual ~Server();
      bool serverUp();
  };
}
