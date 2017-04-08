#include <cstdlib>
#include <cstring>
#include <unistd.h>

#include <application.hpp>
#include <exceptions.hpp>
#include <server.hpp>
#include <client.hpp>
#include <ui.hpp>

#include <cursesm.h>

#define DEFAULT_PORT "11122"

static bool with_server = false;
static bool with_client = false;

//
//===========================================================================
// Main Application Class derived from ncurseses one
//===========================================================================
//

scom::Application::Application()
: NCursesApplication(false)
{
}

void scom::Application::handleArgs(int argc, char* argv[])
{
  // default mode is local server with client
  if(argc == 1)
  {
    port = DEFAULT_PORT;
    host = "localhost";
    with_server = with_client = true;
  }
  else if(argc == 2)
  {
    if((strcmp(argv[1], "--serveronly") == 0) ||
       (strcmp(argv[1], "-s") == 0))
    {
      port = DEFAULT_PORT;
      host = "localhost";
      with_server = true;
    }
    else
    {
      port = DEFAULT_PORT;
      host = argv[1];
      with_client = true;
    }
  }
}

int scom::Application::run()
{
  NCursesPanel *appstd = new NCursesPanel();
  appstd->box();

  NCursesPanel *menu = new NCursesPanel(
      appstd->height() - 2,
      15,
      1,
      1);
  menu->box();

  scom::TextDuplex *textFields = new scom::TextDuplex(
      appstd->height() - 2,
      appstd->width() - 15 - 2,
      1,
      16,
      6);

  scom::Server *server;
  scom::Client *client;

  if(with_server)
  {
    server = new scom::Server(
        "localhost",
        DEFAULT_PORT);
  }

  if(with_client)
  {
    client = new scom::Client(host, port, textFields);
    if(with_server)
    {
      while(!server->serverUp())
        sleep(1000);
    }
    client->getSocket()->connect();
  }

  appstd->refresh();
  ::echo();
  while(with_client)
  {
    const char* msg = textFields->read();
    if(strcmp(msg, "ZZ") == 0)
    {
      delete client;
      break;
    }
    client->send(msg);
    textFields->print(msg);
  }
  if(!with_client && with_server)
  {
    ::getch();
    delete server;
  }
  else if(with_server)
  {
    delete server;
  }

  delete menu;
  delete textFields;
  delete appstd;
  return EXIT_SUCCESS;
}
