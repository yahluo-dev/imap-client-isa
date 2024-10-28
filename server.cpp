#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdexcept>
#include <stdio.h>
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
    std::cerr << "getaddrinfo: " << gai_strerror(rv);
    fflush(stderr);
    throw std::runtime_error("Connection failure");
  }

  for(p = server_addrinfo; p != NULL; p = p->ai_next) {
    if ((client_socket = socket(p->ai_family, p->ai_socktype,
                                p->ai_protocol)) == -1) {
      perror("client: socket");
      fflush(stderr);
      throw std::runtime_error("Connection failure");
      continue;
    }

    if (connect(client_socket, p->ai_addr, p->ai_addrlen) == -1) {
      close(client_socket);
      perror("client: connect");
      fflush(stderr);
      throw std::runtime_error("Connection failure");
      continue;
    }
  }

  // This ensures that the receiver eventually leaves
  // uninterruptible sleep upon calling recv()
  struct timeval tv;
  tv.tv_sec = TIMEOUT_S;
  tv.tv_usec = 0;
  setsockopt(client_socket, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));
}

int Server::get_socket()
{
  return client_socket;
}

void Server::send(std::unique_ptr<Command> command)
{
  std::string to_send = command->make_tcp();
  ::send(client_socket, to_send.c_str(), to_send.size(), 0);
}

Server::~Server()
{
  close(client_socket);
}
