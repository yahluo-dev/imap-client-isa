/*
 * @file client.hpp
 * @brief Class implementing a REPL to interact with the user.
 * @author Iaroslav Vasilevskii (xvasil10)
 */
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
    static inline const std::regex DOWNLOADNEW = std::regex("DOWNLOADNEW( ([a-zA-Z0-9-_]{1,128}))?",
                                                            std::regex_constants::ECMAScript);
    static inline const std::regex DOWNLOADALL = std::regex("DOWNLOADALL( ([a-zA-Z0-9-_]{1,128}))?",
                                                            std::regex_constants::ECMAScript);
    static inline const std::regex READNEW = std::regex("READNEW( ([a-zA-Z0-9-_]{1,128}))?",
                                                        std::regex_constants::ECMAScript);
  };
  std::unique_ptr<Session> session;
  void print_prompt();
  Logger logger;
  void save_mail(std::vector<std::string> messages);
  void process_command(const std::string &input);
  DirectoryWriter directoryWriter;
  void download_new(const std::smatch &match);
  void download_all(const std::smatch &match);
  void read_new(const std::smatch &match);
public:
  void repl();
  Client(std::unique_ptr<Session> _session, std::string _mail_dir);
};

#endif // CLIENT_H_
