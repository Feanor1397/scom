#include <server.hpp>
#include <server_socket.hpp>
#include <exceptions.hpp>
#include <iostream>
#include <unistd.h>

static bool server_up = false;

int scom::Server::find_free_place_for_user(
    std::vector<struct scom::Server::userlist_s>* _userlist)
{
  int size = _userlist->size();
  if(size == 0)
    return 0;
  else
  {
    for(int i = 0; i < size; i++)
      if(_userlist->at(i).uid != i)
        return i;
    return size;
  }
}

int scom::Server::new_user(const char* name,
    std::vector<scom::Server::userlist_s>* userlist)
{
  struct userlist_s user;
  int uid = find_free_place_for_user(userlist);
  user.name = name;
  user.uid = uid;
  userlist->insert(userlist->begin() + uid, user);
  return uid;
}

void *scom::Server::serverRoutine(void* _args)
{
  struct ServArgs* args = (ServArgs*)_args;
  const char* port = args->port;
  std::vector<struct scom::Server::userlist_s>* userlist = args->userlist;

  fd_set master;
  fd_set read_fds;
  int fdmax;

  FD_ZERO(&master);
  FD_ZERO(&read_fds);

  scom::ServerSocket* server = new scom::ServerSocket(port);

  FD_SET(server->listenerFD(), &master);
  fdmax = server->listenerFD();

  server_up = true;

  for(;;)
  {
    if(serverLock)
      break;

    read_fds = master;
    struct timeval tv;
    tv.tv_sec = 0;
    tv.tv_usec = 100000;
    if(select(fdmax + 1, &read_fds, NULL, NULL, &tv) == -1)
      break;

    for(int i = 0; i <= fdmax; i++)
    {
      if(FD_ISSET(i, &read_fds))
      {
        if(i == server->listenerFD())
        {
          int newfd = server->acceptConnection();
          if(newfd == -1)
            break;
          else
          {
            FD_SET(newfd, &master);
            if(newfd > fdmax)
              fdmax = newfd;
          }
        }
        else
        {
          const char* buff;
          try
          {
            // new message. resend it to all clients
            buff = server->recv(i);
            type_t type = scom::get_type(buff);
            if(type == scom::NORMAL_MESSAGE)
            {
              for(int j = 0; j <= fdmax; j++)
                if(FD_ISSET(j, &master))
                  if(j != server->listenerFD() && j != i)
                    server->send(j, buff);
            }
            else if(type == scom::IM_MESSAGE)
            {
              /* TODO */
            }
            else if(type == scom::AUTH_REQUEST)
            {
              const char* name = scom::get_name_from_request(buff);
              int uid = scom::Server::new_user(name, userlist);
              const char* resp = scom::create_auth_response(uid);
              server->send(i, resp);
              delete[] name;
              delete[] resp;
              /* send userlist to all */
            }
            delete[] buff;
          }
          catch(scom::ConnectionClosed)
          {
            FD_CLR(i, &master);
            close(i);
          }
        }
      }
    }
  }
  delete server;
  delete args;
  pthread_exit(NULL);
}

scom::Server::Server(
    const char* _port)
{
  ServArgs* args = new ServArgs;
  args->port = _port;
  args->userlist = &userlist;
  pthread_create(&id, NULL, serverRoutine, (void*)args);
}

scom::Server::~Server()
{
  serverLock = true;
  pthread_join(id, NULL);
}

bool scom::Server::serverUp()
{
  return server_up;
}
