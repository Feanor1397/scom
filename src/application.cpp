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
      port = DEFAULT_PORT;
      host = argv[1];
      with_client = true;
  }
}

void scom::Application::title()
{
  const char* titleText = "SCOM";
  const int titleLen = strlen(titleText);
  titleWindow->bkgd(screen_titles());
  titleWindow->addstr(0, (titleWindow->width() - titleLen) / 2, titleText);
  titleWindow->noutrefresh();
}

int scom::Application::run()
{
  scom::Server *server;
  scom::Client *client;

  if(with_server)
  {
    server = new scom::Server(DEFAULT_PORT);
  }

  if(with_client)
  {
    client = new scom::Client(host, port);
    if(with_server)
    {
      while(!server->serverUp())
        sleep(1000);
    }
    client->getSocket()->connect();

    curs_set(0);
    raw();

    bool exit = false;
    while(!exit)
    {
      int ch = getch();
      switch(ch)
      {
        case '\n':
        case '\r':
          client->send();
          break;
        case 'Z':
        {
          int ch2 = getch();
          switch(ch2)
          {
            case 'Z':
            case 'Q':
              delete client;
              exit = true;
              break;
          }
          break;
        }
        default:
          break;
      }
    }
  }

  if(with_server)
    delete server;

  return EXIT_SUCCESS;
}
