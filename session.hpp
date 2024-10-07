#ifndef SESSION_H_
#define SESSION_H_

#include <string>
#include <memory>
#include "server.hpp"

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
  std::unique_ptr<Server> server;
  public:
  Session(const std::string hostname, const std::string port);
  void Login(const std::string _username, const std::string _password);
};

#endif // SESSION_H_
