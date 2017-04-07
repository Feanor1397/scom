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
    throw scom::Exception();

  if((i32SocketFD = socket(ai->ai_family, ai->ai_socktype, ai->ai_protocol)) == -1)
    throw scom::Exception();

  if((status = bind(i32SocketFD, ai->ai_addr, ai->ai_addrlen)) == -1)
    throw scom::Exception();

  if((status = listen(i32SocketFD, 10)) == -1)
    throw scom::Exception();
}

scom::ServerSocket::~ServerSocket()
{
  freeaddrinfo(ai);
  close(i32SocketFD);
}

int scom::ServerSocket::acceptConnection()
{
  int i32ConnectFD = ::accept(i32SocketFD, 0, 0);
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
  {
    perror("recv");
    throw scom::Exception();
  }
  else if(status == 0)
    throw scom::ConnectionClosed();

  int len = ntohs(to_recv[0]<<8 | to_recv[1]);

  status = ::recv(connection, to_recv+2, len, 0);
  if(status == -1)
    throw scom::Exception();
  else if(status == 0)
    throw scom::ConnectionClosed();

  to_return = to_recv+2;

  return to_return;
}

void scom::ServerSocket::send(int connection, const char* message)
{
  ::send(connection, message, strlen(message), 0);
}

int scom::ServerSocket::listenerFD()
{
  return i32SocketFD;
}
