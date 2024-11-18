#ifndef CLIENT_H_
#define CLIENT_H_

#include <memory>
#include "session.hpp"
#include "directory_writer.hpp"


class Client
{
protected:
  const std::string HELP = "DOWNLOADALL [mailbox]\tDownload all mail in mailbox\n"
                                  "DOWNLOADNEW [mailbox]\tDownload all new mail in mailbox\n"
                                  "READNEW [mailbox]\tMark all mail as read in mailbox\n"
                                  "QUIT\tLogout and quit the program.\n"
                                  "HELP\tShow this message\n";
  struct Commands
  {
    static const std::regex DOWNLOADNEW;
    static const std::regex DOWNLOADALL;
    static const std::regex READNEW;
  };
  std::unique_ptr<Session> session;
  void print_prompt();
  Logger logger;
  void save_mail(std::vector<std::string> messages);
  DirectoryWriter directoryWriter;
public:
  void repl();
  Client(std::unique_ptr<Session> _session, std::string _mail_dir);
};

#endif // CLIENT_H_
