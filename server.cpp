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

Server::Server(const std::string hostname, const std::string port)
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
}


void Server::send(std::unique_ptr<Command> command)
{
  std::string to_send = command->make_tcp();
  ::send(client_socket, to_send.c_str(), to_send.size(), 0);
}

std::unique_ptr<Response> Server::receive()
{
  char buffer[2048];
  std::string response_data;

  ssize_t bytes_recvd = recv(client_socket, &buffer, 2048, 0);
  response_data = std::string(buffer, bytes_recvd);

  std::unique_ptr<TextResponse> response = std::make_unique<TextResponse>(response_data); // TODO

  return std::move(response);
}

TLSServer::TLSServer(const std::string hostname, const std::string port)
  : Server(hostname, port)
{
  throw std::logic_error("Not implemented");
}
