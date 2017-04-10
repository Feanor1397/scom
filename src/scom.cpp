#include <application.hpp>
#include <server.hpp>

#include <iostream>
#include <cstdlib>
#include <cstring>
#include <locale.h>

#define DEFAULT_PORT "11122"

int main(int argc, char* argv[])
{
  int res = 0;

  setlocale(LC_ALL, "");

  if(argc == 2 && (strcmp(argv[1], "-s") == 0 ||
                   strcmp(argv[1], "--server") == 0))
  {
    scom::Server* server = new scom::Server(DEFAULT_PORT);
    bool exit = false;
    while(!exit)
    {
      char input[64];
      std::cin >> input;
      if(strcmp(input, "exit") == 0)
      {
        delete server;
        exit = true;
      }
    }
  }
  else
  {
    try
    {
      scom::Application app;
      app.handleArgs(argc, argv);
      res = app();
      endwin();
    }
    catch(const NCursesException *e)
    {
      endwin();
      std::cerr << e->message << std::endl;
      res = e->errorno;
    }
    catch(const NCursesException &e)
    {
      endwin();
      std::cerr << e.message << std::endl;
      res = e.errorno;
    }
    catch(const std::exception &e)
    {
      endwin();
      std::cerr << "Exception: " << e.what() << std::endl;
      res = EXIT_FAILURE;
    }
  }

  return res;
}
