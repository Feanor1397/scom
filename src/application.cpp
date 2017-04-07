#include <cstdlib>
#include <cstring>
#include <application.hpp>
#include <exceptions.hpp>
#include <server.hpp>
#include <client.hpp>

#include <cursesm.h>

#include <ui.hpp>
#define DEFAULT_PORT "11122"

scom::Application::Application()
: NCursesApplication(false)
{
}

void scom::Application::handleArgs(int argc, char* argv[])
{
  if(argc == 0)
  {
    port = DEFAULT_PORT;
    host = "localhost";
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

  scom::Server *server = new scom::Server("localhost", DEFAULT_PORT, textFields);

  appstd->refresh();
  ::echo();
  while(1)
  {
    const char* msg = textFields->read();
    if(strcmp(msg, "ZZ") == 0)
      break;
    textFields->print(msg);
  }
  delete menu;
  delete textFields;
  delete appstd;
  return EXIT_SUCCESS;
}
