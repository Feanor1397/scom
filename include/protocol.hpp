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

  /* Text */
  const char* get_text(int skip, const char* message);
  int get_sender_uid(const char* message);
  int get_receiver_uid(const char* message);
  struct message_s parse_message(const char* message);

  const char* create_message(int uid, const char* message);
  const char* create_im_message(int uid, int target_uid, const char* message);
  const char* create_error_message(error_t type);
  const char* create_system_message(const char* message);

  /* User list */
  const char** get_user_list(const char* message);
  const char* create_user_list_message(const char** users);
}
