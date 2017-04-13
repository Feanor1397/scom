#pragma once

namespace scom
{
  enum type_t
  {
    AUTH_REQUEST,
    CLIENT_USER_ID,
    NORMAL_MESSAGE,
    IM_MESSAGE,
    SYSTEM_MESSAGE,
    ERROR,
    USERLIST_REQUEST,
    USERLIST,
    DISCONNECT
  };

  enum error_t
  {
    SERVER_FULL,
    NICKNAME_ALREADY_USED
  };

  struct message_s
  {
    type_t type;
    int sender;
    const char* message;
  };

  enum type_t get_type(const char* message);

  const char* get_text(int skip, const char* message);
  int get_sender_uid(const char* message);
  int get_receiver_uid(const char* message);
  struct message_s parse_message(const char* message);

  /* Auth */
  const char* create_auth_request(const char* name);
  const char* create_auth_response(int uid);
  const char* get_name_from_request(const char* message);
  int get_uid_from_response(const char* message);

  /* Text */
  const char* create_message(int uid, const char* message);
  const char* create_im_message(int uid, int target_uid, const char* message);
  const char* create_error_message(error_t type);
  const char* create_system_message(const char* message);
}
