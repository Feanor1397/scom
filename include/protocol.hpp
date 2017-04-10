#pragma once

namespace scom
{
  enum type_t
  {
    TEXT,
    USERLIST,
    SYSTEM,
    ERROR
  };

  enum type_t get_type(const char* message);

  /* Text */
  const char* get_text(const char* message);
  const char* create_message(int uid, const char* message);
  const char* create_error_message(const char* message);
  const char* create_system_message(const char* message);

  /* User list */
  const char** get_user_list(const char* message);
  const char* create_user_list_message(const char** users);
}
