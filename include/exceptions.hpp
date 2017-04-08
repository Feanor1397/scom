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
      Exception();
      virtual void printInfo();
  };
  class ConnectionClosed : public Exception
  {
    public:
      ConnectionClosed();
  };
  class GetAddrInfo : public Exception{};
  class CreateSocket : public Exception{};
  class Bind : public Exception{};
  class Listen : public Exception{};
  class Accept : public Exception{};
  class Recv : public Exception{};
  class Send : public Exception{};
}
