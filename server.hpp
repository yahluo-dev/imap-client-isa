/**
 * @file server.hpp
 * @brief Class initializing the socket and managing message sending
 * @author Iaroslav Vasilevskii (xvasil10)
 */
#ifndef SERVER_H_
#define SERVER_H_

#include <memory>
#include <queue>
#include <thread>

#include "command.hpp"
#include "response.hpp"
#include "logger.hpp"
#include "receiver.hpp"

class Session;

class Server
{
  protected:
  std::queue<std::unique_ptr<Response>> response_buffer;
  int client_socket;
  Logger logger;
  std::thread receiving_thread;
  std::unique_ptr<Receiver> receiver;
  void connect_with_timeout(const std::string hostname, const std::string port);
  public:
  Server(const std::string hostname, const std::string port);
  virtual ~Server();
  virtual void send(std::unique_ptr<Command> command);
  int get_socket();
  virtual void receive(Session &session);
};

#endif // SERVER_H_
