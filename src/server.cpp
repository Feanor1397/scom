#include <server.hpp>
#include <exceptions.hpp>
#include <iostream>
#include <cstring>
#include <unistd.h>

scom::ServerSocket::ServerSocket(const char* port)
{
  memset(&hints, 0, sizeof hints);
  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags = AI_PASSIVE;

  if((status = getaddrinfo(NULL, port, &hints, &ai)) != 0)
    throw scom::Exception(status);

  if((i32SocketFD = socket(ai->ai_family, ai->ai_socktype, ai->ai_protocol)) == -1)
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
