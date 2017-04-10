#include <server.hpp>
#include <server_socket.hpp>
#include <exceptions.hpp>
#include <iostream>
#include <unistd.h>

static bool server_up = false;

void *scom::Server::serverRoutine(void* _args)
{
  const char* port = (const char*)_args;

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
          try
          {
            // new message. resend it to all clients
            const char* buff = server->recv(i);
            for(int j = 0; j <= fdmax; j++)
            {
              if(FD_ISSET(j, &master))
              {
                if(j != server->listenerFD() && j != i)
                  server->send(j, buff);
              }
            }
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
  pthread_exit(NULL);
}

scom::Server::Server(
    const char* _port)
{
  pthread_create(&id, NULL, serverRoutine, (void*)_port);
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
