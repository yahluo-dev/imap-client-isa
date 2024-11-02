#ifndef CLIENT_H_
#define CLIENT_H_

#include <memory>
#include "session.hpp"


class Client
{
protected:
  struct Commands
  {
    static const std::regex DOWNLOADNEW;
    static const std::regex DOWNLOADALL;
    static const std::regex READNEW;
  };
  std::unique_ptr<Session> session;
  void print_prompt();
  std::string mail_dir;
  Logger logger;
  void save_mail(std::vector<std::string> messages);
public:
  void repl();
  Client(std::unique_ptr<Session> _session, std::string _mail_dir);
};

const std::regex Client::Commands::DOWNLOADNEW("DOWNLOADNEW ([a-zA-Z0-9-_]{1,128})", std::regex_constants::ECMAScript);
const std::regex Client::Commands::DOWNLOADALL("DOWNLOADALL ([a-zA-Z0-9-_]{1,128})", std::regex_constants::ECMAScript);
const std::regex Client::Commands::READNEW("READNEW", std::regex_constants::ECMAScript);

#endif // CLIENT_H_
