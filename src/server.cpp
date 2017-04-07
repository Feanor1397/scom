#include <server.hpp>
#include <exceptions.hpp>
#include <iostream>
#include <cstring>
#include <unistd.h>

static bool server_up = false;
static bool lock = false;

scom::ServerSocket::ServerSocket(const char* port)
{
  memset(&hints, 0, sizeof hints);
  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags = AI_PASSIVE;

  if((status = getaddrinfo(NULL, port, &hints, &ai)) != 0)
    throw scom::Exception(status);

  if((i32SocketFD = socket(ai->ai_family,
                           ai->ai_socktype,
                           ai->ai_protocol)) == -1)
    throw scom::Exception(status);

  if((status = bind(i32SocketFD, ai->ai_addr, ai->ai_addrlen)) == -1)
    throw scom::Exception(status);

  if((status = listen(i32SocketFD, 10)) == -1)
    throw scom::Exception(status);
}

scom::ServerSocket::~ServerSocket()
{
  freeaddrinfo(ai);
  close(i32SocketFD);
}

int scom::ServerSocket::acceptConnection()
{
  int i32ConnectFD = ::accept(i32SocketFD, 0, 0);
  if(i32ConnectFD == -1)
    throw scom::Exception(i32ConnectFD);

  return i32ConnectFD;
}

void scom::ServerSocket::disconnect(int connection)
{
  shutdown(connection, SHUT_RDWR);
}

const char* scom::ServerSocket::recv(int connection)
{
  char to_recv[1024+2];
  memset(to_recv, 0, 1024+2);
  char* to_return;

  status = ::recv(connection, to_recv, 2, 0);
  if(status == -1)
    throw scom::Exception(status);
  else if(status == 0)
    throw scom::ConnectionClosed();

  int len = ntohs(to_recv[0]<<8 | to_recv[1]);

  status = ::recv(connection, to_recv+2, len, 0);
  if(status == -1)
    throw scom::Exception(status);
  else if(status == 0)
    throw scom::ConnectionClosed();

  to_return = to_recv+2;

  return to_return;
}

void scom::ServerSocket::send(int connection, const char* message)
{
  unsigned int total = 0;
  unsigned int len = strlen(message) + 1;

  if(len > 1024)
    throw scom::Exception(status);

  unsigned int bytesleft = len + 2;
  unsigned int n;

  char to_send[len + 2];
  memset(to_send, 0, len + 2);

  uint16_t nlen = htons(len);
  to_send[0] = nlen>>8;
  to_send[1] = nlen;

  strcpy(to_send+2, message);

  while(total < len + 2)
  {
    n = ::send(connection, to_send+total, bytesleft, 0);
    if(n == -1)
      break;
    total += n;
    bytesleft -= n;
  }

  if(n == -1)
    throw scom::Exception(n);
}

int scom::ServerSocket::listenerFD()
{
  return i32SocketFD;
}

//
//===========================================================================
// End of Socket
//===========================================================================
//

void *scom::Server::serverRoutine(void* _args)
{
  const scom::Args* args = (scom::Args*)_args;

  fd_set master;
  fd_set read_fds;
  int fdmax;

  FD_ZERO(&master);
  FD_ZERO(&read_fds);

  scom::ServerSocket* server = new scom::ServerSocket(args->port);

  FD_SET(server->listenerFD(), &master);
  fdmax = server->listenerFD();

  server_up = true;

  for(;;)
  {
    if(lock)
      break;

    read_fds = master;
    if(select(fdmax + 1, &read_fds, NULL, NULL, NULL) == -1)
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
          args->ui->print("I: New connection");
        }
        else
        {
          try
          {
            const char* buff = server->recv(i);
            args->ui->print(buff);
          }
          catch(scom::ConnectionClosed)
          {
            args->ui->print("I: Connection closed");
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
    const char* _host,
    const char* _port,
    scom::TextDuplex *_ui)
{
  args.host = _host;
  args.port = _port;
  args.ui = _ui;

  pthread_create(&id, NULL, serverRoutine, (void*)&args);
}

scom::Server::~Server()
{
  lock = true;
  pthread_join(id, NULL);
}
