#include <protocol.hpp>

#include <cstring>
#include <cstdlib>
#include <cstdio>

enum scom::type_t scom::get_type(const char* message)
{
  char buff[2];
  int result;

  strncpy(buff, message, 1);
  buff[1] = '\0';
  sscanf(buff, "%d", &result);

  return (scom::type_t)result;
}

const char* scom::get_text(const char* message)
{
  int len;
  const char* result;

  len = strlen(message) - 1; /* first byte is type_t */
  char buff[len];
  strncpy(buff, message + 1, len);
  result = buff;

  return result;
}

const char* scom::create_message(int uid, const char* message)
{
  int msg_len;
  int len;
  const char* result;

  msg_len = strlen(message);
  len = msg_len + 3 + 1;

  char buff[len];
  sprintf(buff, "%01d%03d%s", TEXT, uid, message);
  result = buff;

  return result;
}

const char* scom::create_error_message(const char* message)
{
  int msg_len;
  int len;
  const char* result;

  msg_len = strlen(message);
  len = msg_len + 3 + 1;

  char buff[len];
  sprintf(buff, "%01d%03d%s", ERROR, 0, message);
  result = buff;

  return result;
}

const char* scom::create_system_message(const char* message)
{
  int msg_len;
  int len;
  const char* result;

  msg_len = strlen(message);
  len = msg_len + 3 + 1;

  char buff[len];
  sprintf(buff, "%01d%03d%s", SYSTEM, 0, message);
  result = buff;

  return result;
}
