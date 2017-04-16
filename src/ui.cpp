#include <ui.hpp>
#include <cstring>

//
//==========================================================================
//Text Field for entering mesage
//==========================================================================
//

scom::TextField::TextField(int height, int width, int y, int x)
: _height(height),
  _width(width),
  _y(y),
  _x(x)
{
  border = new NCursesPanel(_height, _width, _y, _x);
  border->box();
  field = new NCursesPanel(_height - 2, _width - 2, _y + 1, _x + 1);
  field->scrollok(true);
}

scom::TextField::~TextField()
{
  delete field;
  delete border;
}

const char* scom::TextField::read()
{
  const char* to_ret;
  field->getstr(buff, 256);
  field->clear();
  to_ret = buff;
  return to_ret;
}

//
//==========================================================================
//Text Field for printing mesages
//==========================================================================
//

scom::TextFrame::TextFrame(int height, int width, int y, int x)
: _height(height),
  _width(width),
  _y(y),
  _x(x)
{
  border = new NCursesPanel(_height, _width, _y, _x);
  border->box();
  field = new NCursesPanel(_height - 2, _width - 2, _y + 1, _x + 1);
  field->scrollok(true);
}

scom::TextFrame::~TextFrame()
{
  delete field;
  delete border;
}

void scom::TextFrame::print(const char* name, const char* message)
{
  const int max_name_len = 15;
  int name_len = strlen(name);

  field->addstr(name);

  for(int i = 0; i < max_name_len - name_len; i++)
    field->addch(' ');
  field->addstr("| ");
  field->addstr(message);
  field->addch('\n');
  field->refresh();
}

//
//==========================================================================
//Both Fields inside one object
//==========================================================================
//

scom::TextDuplex::TextDuplex(int height, int width,
                             int y, int x, int f_height)
: _height(height),
  _width(width),
  _y(y),
  _x(x),
  _f_height(f_height)
{
  frame = new scom::TextFrame(_height - _f_height,
                              _width,
                              _y,
                              _x);

  field =new scom::TextField(_f_height,
                             _width,
                             _y + _height - _f_height,
                             _x);
}

scom::TextDuplex::~TextDuplex()
{
  delete field;
  delete frame;
}

void scom::TextDuplex::print(const char* name, const char* message)
{
  frame->print(name, message);
}

const char* scom::TextDuplex::read()
{
  return field->read();
}
