#include <iostream>
#include <memory>

#include "command.hpp"
#include "session.hpp"
#include "server.hpp"

Session::Session(const std::string hostname, const std::string port)
{
  server = std::make_unique<Server>(hostname, port);
}

void Session::Login(const std::string username, const std::string password)
{

  std::unique_ptr<Response> welcome_response = server->receive();

  std::unique_ptr<LoginCommand> login_command = std::make_unique<LoginCommand>("a001", username, password); // FIXME

  // TODO: Put it into some nicer wrapper function

  server->send(std::move(login_command));

  std::unique_ptr<Response> second_response = server->receive();
}
