#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdexcept>
#include <stdio.h>
#include <cstring>
#include <iostream>
#include <memory>

#include "server.hpp"
#include "response.hpp"
#include "response_parser.hpp"

#define TIMEOUT_S 1


Server::Server(const std::string hostname, const std::string port)
  : logger(std::cerr)
{
  int rv;
  struct addrinfo hints = {0};
  struct addrinfo *p;
  struct addrinfo *server_addrinfo;

  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_STREAM;

  if (0 != (rv = getaddrinfo(hostname.c_str(), port.c_str(), &hints, &server_addrinfo)))
  {
    throw std::runtime_error(gai_strerror(rv));
  }

  for(p = server_addrinfo; p != NULL; p = p->ai_next) {
    if ((client_socket = socket(p->ai_family, p->ai_socktype,
                                p->ai_protocol)) == -1) {
      throw std::runtime_error(strerror(errno));
      continue;
    }

    if (connect(client_socket, p->ai_addr, p->ai_addrlen) == -1) {
      close(client_socket);
      throw std::runtime_error(strerror(errno));
      continue;
    }
  }

  // This ensures that the receiver eventually leaves
  // uninterruptible sleep upon calling recv()
  struct timeval tv;
  tv.tv_sec = TIMEOUT_S;
  tv.tv_usec = 0;
  setsockopt(client_socket, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));

  receiver = std::make_unique<Receiver>(client_socket);
}

int Server::get_socket()
{
  return client_socket;
}

void Server::receive(Session &session)
{
  receiving_thread = std::thread([this, &session]() {
    this->receiver->receive(session);
  });
}

void Server::send(std::unique_ptr<Command> command)
{
  std::string to_send = command->make_tcp();
  ::send(client_socket, to_send.c_str(), to_send.size(), 0);
}

Server::~Server()
{
  receiver->stop();
  receiving_thread.join();
  close(client_socket);
}
