#ifndef CLIENT_H_
#define CLIENT_H_

#include <memory>
#include "session.hpp"


class Client
{
protected:
  const std::string HELP = "DOWNLOADALL [mailbox] - Download all mail in mailbox\n"
                                  "DOWNLOADNEW [mailbox] - Download all newmail in mailbox\n"
                                  "READNEW [mailbox] - Mark all mail as read in mailbox\n";
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

#endif // CLIENT_H_
