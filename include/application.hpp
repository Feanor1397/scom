#pragma once
#include <cursesapp.h>
#include <vector>

namespace scom
{
  class Application : public NCursesApplication
  {
    private:
      const char* port;
      const char* host;
      std::vector<const char*> args;
    protected:
      virtual int titlesize() const { return 1; }
    public:
      Application();
      virtual void handleArgs(int argc, char* argv[]);
      virtual int run();
  };
}
