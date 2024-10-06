#ifndef SESSION_H_
#define SESSION_H_

#include <string>

enum imap_state_t
{
  NAUTH,
  AUTHD,
  SELECTED,
};

class Session
{
  protected:
  imap_state_t state;
  std::string username;
  std::string password;
  int socket;
  public:
  Session(const std::string _username, const std::string _password);
};

class TLSSession : public Session
{
  public:
  TLSSession(const std::string _username, const std::string _password);
};

#endif // SESSION_H_
