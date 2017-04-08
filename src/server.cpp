#include <server.hpp>
#include <exceptions.hpp>
#include <iostream>
#include <cstring>
#include <unistd.h>

static bool server_up = false;

//
//===========================================================================
// Server's socket
//===========================================================================
//

scom::ServerSocket::ServerSocket(const char* port)
{
  memset(&hints, 0, sizeof hints);
  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags = AI_PASSIVE;

  if((status = getaddrinfo(NULL, port, &hints, &ai)) != 0)
    throw scom::GetAddrInfo();

  if((i32SocketFD = socket(ai->ai_family,
                           ai->ai_socktype,
                           ai->ai_protocol)) == -1)
    throw scom::CreateSocket();

  if((status = bind(i32SocketFD, ai->ai_addr, ai->ai_addrlen)) == -1)
    throw scom::Bind();

  if((status = listen(i32SocketFD, 10)) == -1)
    throw scom::Listen();
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
    throw scom::Accept();

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
    throw scom::Recv();
  else if(status == 0)
    throw scom::ConnectionClosed();

  int len = ntohs(to_recv[0]<<8 | to_recv[1]);

  status = ::recv(connection, to_recv+2, len, 0);
  if(status == -1)
    throw scom::Recv();
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
    throw scom::Exception();

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
    throw scom::Send();
}

int scom::ServerSocket::listenerFD()
{
  return i32SocketFD;
}

//
//===========================================================================
// Server class. Each object is separate thread
//===========================================================================
//

void *scom::Server::serverRoutine(void* _args)
{
  const scom::ServArgs* args = (scom::ServArgs*)_args;

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
    const char* _host,
    const char* _port)
{
  args.host = _host;
  args.port = _port;

  pthread_create(&id, NULL, serverRoutine, (void*)&args);
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
