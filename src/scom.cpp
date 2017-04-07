#include <application.hpp>
#include <iostream>
#include <cstdlib>

int main(int argc, char* argv[])
{
  int res;

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

  return res;
}
