#ifndef SERVER_H_
#define SERVER_H_

#include <memory>

#include "command.hpp"
#include "response.hpp"

class Server
{
  private:
  int client_socket;
  public:
  Server(const std::string hostname, const std::string port);
  void send(std::unique_ptr<Command> command);
  std::unique_ptr<Response> receive();
};

class TLSServer : public Server
{
  private:
  public:
  TLSServer(const std::string hostname, const std::string port);
};

#endif // SERVER_H_
