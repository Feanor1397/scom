#include <client.hpp>
#include <exceptions.hpp>
#include <ui.hpp>
#include <protocol.hpp>

#include <iostream>
#include <cstring>
#include <unistd.h>

static bool lock = false;

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
        scom::message_s message = scom::parse_message(clientSocket->recv());
        ui->print(message.message);
      }
      catch(scom::ConnectionClosed)
      {
        FD_CLR(fdmax, &master);
        close(fdmax);
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
  socket->connect();

  /* wait for socket available to write */
  fd_set set;
  FD_ZERO(&set);
  FD_SET(socket->getFD(), &set);
  select(socket->getFD() + 1, NULL, &set, NULL, NULL);
  const char* request = scom::create_auth_request("User");
  socket->send(request);

  /* wait for server's auth response */
  fd_set response;
  FD_ZERO(&response);
  FD_SET(socket->getFD(), &response);
  select(socket->getFD() + 1, &response, NULL, NULL, NULL);
  uid = scom::get_uid_from_response(socket->recv());

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

void scom::Client::user_input()
{
  echo();
  curs_set(1);

  const char* input = textFields->read();

  if(input[0] == '/') /* special comands */
  {
  }
  else /* normal message */
  {
    send(input);
    textFields->print(input);
  }

  curs_set(0);
  noecho();
}

void scom::Client::send(const char* message)
{
  const char* msg;
  msg = scom::create_message(uid, message);
  socket->send(msg);
}

void scom::Client::send_im(int target_uid, const char* message)
{
  const char* msg;
  msg = scom::create_im_message(uid, target_uid, message);
  socket->send(msg);
}

scom::ClientSocket* scom::Client::getSocket()
{
  return socket;
}
