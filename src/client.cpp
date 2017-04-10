#include <client.hpp>
#include <exceptions.hpp>
#include <ui.hpp>

#include <iostream>
#include <cstring>
#include <unistd.h>

static bool lock = false;

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
    tv.tv_usec = 50000;
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
