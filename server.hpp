#ifndef SERVER_H_
#define SERVER_H_

#include <memory>
#include <queue>

#include "command.hpp"
#include "response.hpp"
#include "logger.hpp"

class Server
{
  protected:
  std::queue<std::unique_ptr<Response>> response_buffer;
  int client_socket;
  static constexpr size_t RECVMESSAGE_MAXLEN = 4096;
  Logger logger;
  public:
  Server(const std::string hostname, const std::string port);
  ~Server();
  virtual void send(std::unique_ptr<Command> command);
  int get_socket();
};

#endif // SERVER_H_
