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
  uint32_t tag;
  std::string username;
  std::string password;
  std::unique_ptr<Server> server;
  std::string get_new_tag();

  public:
  Session(const std::string _hostname, const std::string _port);
  void login(const std::string username, const std::string password);
  void select(const std::string mailbox);
  std::vector<uint32_t> search();
  std::vector<std::string> fetch(std::vector<uint32_t> sequence_set);
};

#endif // SESSION_H_
