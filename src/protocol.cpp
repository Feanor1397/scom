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

int scom::get_sender_uid(const char* message)
{
  char buff[4];
  int result;

  strncpy(buff, message + 1, 3);
  buff[3] = '\0';
  sscanf(buff, "%03d", &result);

  return result;
}

int scom::get_receiver_uid(const char* message)
{
  char buff[4];
  int result;

  strncpy(buff, message + 4, 3);
  buff[3] = '\0';
  sscanf(buff, "%03d", &result);

  return result;
}

const char* scom::get_text(int skip, const char* message)
{
  int len;
  const char* result;

  len = strlen(message) - skip;
  char* buff = new char[len];
  strncpy(buff, message + skip, len);
  result = buff;

  return result;
}

struct scom::message_s scom::parse_message(const char* message)
{
  struct message_s msg;

  msg.type = get_type(message);
  msg.sender = get_sender_uid(message);
  if(msg.type == NORMAL_MESSAGE)
    msg.message = get_text(1 + 3, message); /* one 3-byte uid */
  else
    msg.message = get_text(1 + 3 + 3, message); /* two 3-byte uids */

  return msg;
}

const char* scom::create_auth_request(const char* name)
{
  const char* result;
  int len;

  len = strlen(name);
  char* buff = new char[1 + len];
  sprintf(buff, "%01d%s", AUTH_REQUEST, name);
  result = buff;
  return result;
}

const char* scom::create_auth_response(int uid)
{
  const char* result;
  char* buff = new char[5];
  sprintf(buff, "%01d%03d", CLIENT_USER_ID, uid);
  result = buff;
  return result;
}

const char* scom::get_name_from_request(const char* message)
{
  const char* result;
  int len = strlen(message) - 1;
  char* buff = new char[len];
  strncpy(buff, message + 1, len);
  result = buff;
  return result;
}

int scom::get_uid_from_response(const char* message)
{
  int result;
  char buff[4];

  strncpy(buff, message + 1, 3);
  buff[3] = '\0';
  sscanf(buff, "%03d", &result);

  return result;
}

const char* scom::create_message(int uid, const char* message)
{
  int msg_len;
  int len;
  const char* result;

  msg_len = strlen(message);
  len = msg_len + 1 + 3;

  char* buff = new char[len];
  sprintf(buff, "%01d%03d%s", NORMAL_MESSAGE, uid, message);
  result = buff;

  return result;
}

const char* scom::create_im_message(int uid, int target_uid, const char* message)
{
  int msg_len;
  int len;
  const char* result;

  msg_len = strlen(message);
  len = msg_len + 1 + 3 + 3;

  char* buff = new char[len];
  sprintf(buff, "%01d%03d%03d%s", IM_MESSAGE, uid, target_uid, message);
  result = buff;

  return result;
}

const char* scom::create_error_message(error_t type)
{
  const char* result;

  char* buff = new char[2];
  sprintf(buff, "%01d%01d", ERROR, type);
  result = buff;

  return result;
}

const char* scom::create_system_message(const char* message)
{
  int msg_len;
  int len;
  const char* result;

  msg_len = strlen(message);
  len = msg_len + 1;

  char* buff = new char[len];
  sprintf(buff, "%01d%s", SYSTEM_MESSAGE, message);
  result = buff;

  return result;
}
