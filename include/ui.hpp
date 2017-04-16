#pragma once
#include <cursesw.h>
#include <cursesm.h>

namespace scom
{
  class TextField
  {
    private:
      NCursesPanel *border, *field;
      char buff[256];
      int _height, _width;
      int _y, _x;
    public:
      TextField(int height, int width, int y, int x);
      virtual ~TextField();
      const char* read();
  };

  class TextFrame
  {
    private:
      NCursesPanel *border, *field;
      int _height, _width;
      int _y, _x;
    public:
      TextFrame(int height, int width, int y, int x);
      virtual ~TextFrame();
      void print(const char* name, const char* message);
  };

  class TextDuplex
  {
    private:
      TextField *field;
      TextFrame *frame;
      int _height, _width;
      int _y, _x;
      int _f_height;
    public:
      TextDuplex(int height, int width, int y, int x, int f_height);
      virtual ~TextDuplex();
      void print(const char* name, const char* message);
      const char* read();
  };

  class Menu{};
}
