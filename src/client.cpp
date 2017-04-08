#include <client.hpp>
#include <exceptions.hpp>
#include <ui.hpp>

#include <pthread.h>

#include <iostream>
#include <cstring>
#include <unistd.h>

static bool lock = false;

//
//===========================================================================
// Client's socket
//===========================================================================
//

scom::ClientSocket::ClientSocket(const char* host,
                                 const char* port)
{
  memset(&hints, 0, sizeof hints);
  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags = AI_PASSIVE;

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
}

void scom::ClientSocket::disconnect()
{
  if((status = ::shutdown(i32SocketFD, SHUT_RDWR)) == -1)
    throw scom::Exception(status);
}

void scom::ClientSocket::send(const char* message)
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
    n = ::send(i32SocketFD, to_send+total, bytesleft, 0);
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

//
//===========================================================================
// Client class. Manages message fields, incloming msgs in separate tread
//===========================================================================
//

void *scom::Client::clientRoutine(void* _args)
{
  scom::ClientArgs* args = (scom::ClientArgs*)_args;
  scom::ClientSocket* clientSocket = args->sock;
  scom::TextDuplex* ui = args->ui;

  fd_set master;
  fd_set read_fds;
  int fdmax;

  FD_ZERO(&master);
  FD_ZERO(&read_fds);

  FD_SET(clientSocket->getFD(), &master);
  fdmax = clientSocket->getFD();

  for(;;)
  {
    if(lock)
      break;

    read_fds = master;

    struct timeval tv;
    tv.tv_sec = 0;
    tv.tv_usec = 500000;
    if(select(fdmax + 1, &read_fds, NULL, NULL, &tv) == -1)
      break;

    if(FD_ISSET(fdmax, &read_fds))
    {
      try
      {
        const char* buff = clientSocket->recv();
        ui->print(buff);
      }
      catch(scom::ConnectionClosed)
      {
        FD_CLR(fdmax, &master);
        ui->print("Server closed connection");
      }
      catch(scom::Exception &e)
      {
        e.printInfo();
        continue;
      }
    }
  }

  pthread_exit(NULL);
}

scom::Client::Client(const char* host, const char* port)
{
  appstd = new NCursesPanel();

  menu = new NCursesPanel(
      appstd->height(),
      15,
      0,
      0);
  menu->box();

  textFields = new scom::TextDuplex(
      appstd->height(),
      appstd->width() - 15,
      0,
      15,
      6);

  appstd->refresh();

  socket = new scom::ClientSocket(host, port);
  args.sock = socket;
  args.ui = textFields;

  pthread_create(&id, NULL, clientRoutine, (void*)&args);
}

scom::Client::~Client()
{
  curs_set(1);
  lock = true;
  pthread_join(id, NULL);
  delete socket;
  delete menu;
  delete textFields;
  delete appstd;
}

void scom::Client::send()
{
  echo();
  curs_set(1);
  const char* message = textFields->read();
  textFields->print(message);
  socket->send(message);
  curs_set(0);
  noecho();
}

scom::ClientSocket* scom::Client::getSocket()
{
  return socket;
}
