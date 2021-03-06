#include <client_socket.hpp>
#include <exceptions.hpp>

#include <cstring>
#include <unistd.h>

scom::ClientSocket::ClientSocket(const char* host,
                                 const char* port)
{
  memset(&hints, 0, sizeof hints);
  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_STREAM;

  if((status = getaddrinfo(host, port, &hints, &ai)) != 0)
    throw scom::Exception(status);

  if((i32SocketFD = socket(ai->ai_family,
                           ai->ai_socktype,
                           ai->ai_protocol)) == -1)
    throw scom::Exception(i32SocketFD);
}

scom::ClientSocket::~ClientSocket()
{
  freeaddrinfo(ai);
  close(i32SocketFD);
}

void scom::ClientSocket::connect()
{
  for(p = ai; p != NULL; p = p->ai_next)
  {
    status = ::connect(i32SocketFD, p->ai_addr, p->ai_addrlen);
    if(status == -1)
    {
      perror("Connect");
      continue;
    }
    break;
  }
  if(status == -1)
    throw scom::Exception();
}

void scom::ClientSocket::disconnect()
{
  if((status = ::shutdown(i32SocketFD, SHUT_RDWR)) == -1)
    throw scom::Exception(status);
}

void scom::ClientSocket::send(const char* message)
{
  unsigned int total = 0;
  unsigned int len = strlen(message);

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
    n = ::send(i32SocketFD, to_send+total, bytesleft, MSG_NOSIGNAL);
    if(n == -1)
      break;
    total += n;
    bytesleft -= n;
  }

  if(n == -1)
    throw scom::Exception(n);
}

const char* scom::ClientSocket::recv()
{
  char to_recv[1024+2];
  memset(to_recv, 0, 1024+2);
  char* to_return;

  status = ::recv(i32SocketFD, to_recv, 2, 0);
  if(status == -1)
    throw scom::Exception(status);
  else if(status == 0)
    throw scom::ConnectionClosed();

  int len = ntohs(to_recv[0]<<8 | to_recv[1]);

  status = ::recv(i32SocketFD, to_recv+2, len, 0);
  if(status == -1)
    throw scom::Exception(status);
  else if(status == 0)
    throw scom::ConnectionClosed();

  to_return = to_recv+2;

  return to_return;
}

int scom::ClientSocket::getFD()
{
  return i32SocketFD;
}
