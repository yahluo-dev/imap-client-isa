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
#include "credential.hpp"
#include "logger.hpp"

class Receiver;

enum class ImapState
{
  GREETING,
  NAUTH,
  AUTHD,
  SELECTED,
  LOGOUT,
  ERROR // Not a protocol state, signifies a connection error.
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
  std::unique_ptr<Receiver> receiver;
  std::string get_new_tag();
  void transition(ImapState state);
  Logger logger;
  void unexpected_response(std::unique_ptr<Response> &response);
  static std::queue<std::unique_ptr<Response>> response_queue;

  static std::condition_variable incoming_cv;
  static std::mutex incoming_mutex;
  std::unique_ptr<Response> wait_for_response();
  std::thread receiving_thread;
  std::exception receiver_ex;

  public:
  ImapState get_state();
  Session(std::unique_ptr<Server> _server, std::unique_ptr<Receiver> _receiver);
  ~Session();
  void login(Credentials &creds);
  void select(const std::string mailbox);
  void bye();
  std::vector<uint32_t> search(bool only_unseen);
  std::vector<std::string> fetch(std::vector<uint32_t> sequence_set,bool only_headers);
  void receive_greeting();
  void notify_incoming(std::unique_ptr<Response> response);
  void receiver_notify_failed(std::exception &ex);
  void read_new(std::vector<uint32_t> sequence_set);
};

#endif // SESSION_H_
