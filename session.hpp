#ifndef SESSION_H_
#define SESSION_H_

#include <string>
#include <memory>
#include <condition_variable>
#include <mutex>
#include <queue>
#include <thread>
#include "receiver.hpp"
#include "server.hpp"
#include "logger.hpp"

enum class ImapState
{
  GREETING,
  NAUTH,
  AUTHD,
  SELECTED,
  LOGOUT
};

std::ostream& operator<<(std::ostream& os, ImapState response_type);

class Session
{
  protected:
  ImapState state;
  uint32_t tag;
  std::string username;
  std::string password;
  std::unique_ptr<Server> server;
  std::string get_new_tag();
  void transition(ImapState state);
  Logger logger;
  void unexpected_response(std::unique_ptr<Response> &response);
  static std::queue<std::unique_ptr<Response>> response_queue;

  static std::condition_variable incoming_cv;
  static std::mutex incoming_mutex;

  std::unique_ptr<Response> wait_for_response();

  std::thread receiving_thread;

  public:
  Session(std::unique_ptr<Server> _server);
  ~Session();
  void login(const std::string username, const std::string password);
  void select(const std::string mailbox);
  std::vector<uint32_t> search(bool only_unseen);
  std::vector<std::string> fetch(std::vector<uint32_t> sequence_set,bool only_headers);
  void receive_greeting();
  void notify_incoming(std::unique_ptr<Response> response);
};

#endif // SESSION_H_
