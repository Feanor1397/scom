#pragma once

#include <cstdio>

namespace scom
{
  class Exception
  {
    protected:
      int retValue;
    public:
      Exception(const int _retValue);
      virtual void printInfo();
  };
  class ConnectionClosed : public Exception
  {
    public:
      ConnectionClosed();
  };
}
