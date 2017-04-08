#include <exceptions.hpp>
#include <iostream>

scom::Exception::Exception(const int _retValue)
{
  retValue = _retValue;
}

scom::Exception::Exception()
{
  retValue = -1;
}

void scom::Exception::printInfo()
{
  perror("Exception");
  std::cerr << "Function returned " << retValue << std::endl;
}

scom::ConnectionClosed::ConnectionClosed()
: scom::Exception(0)
{
}
