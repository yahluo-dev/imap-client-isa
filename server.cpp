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

#define RECVMESSAGE_MAXLEN 4096

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
  std::unique_ptr<Response> returned;
  char buffer[RECVMESSAGE_MAXLEN];
  std::string responses_data;
  std::vector<std::unique_ptr<Response>> parsed_responses;
  ssize_t bytes_recvd;

  if (!response_buffer.empty())
  {
    returned = std::move(response_buffer.front());
    response_buffer.pop();
    return returned; // If there's something in buffer, return it
  }

  if (-1 == (bytes_recvd = recv(
      client_socket, &buffer, RECVMESSAGE_MAXLEN, 0)))
  {
    throw std::runtime_error("recv() failed");
  }
  if (bytes_recvd == 0)
    throw std::logic_error("Connection already terminated!");
  responses_data.append(buffer, bytes_recvd);
  if (responses_data.size() == RECVMESSAGE_MAXLEN &&
      responses_data.substr(responses_data.size()-2, responses_data.size()) != "\r\n")
  {
    // We didn't receive the entire thing
    throw std::logic_error("Not implemented");
  }

  ResponseParser response_parser = ResponseParser(responses_data);
  std::unique_ptr<Response> response;
  while(response_parser.parse_next(response))
  {
    response_buffer.push(std::move(response));
  }

  returned = std::move(response_buffer.front());
  response_buffer.pop();
  return returned; // Return first of the newly parsed messages

}

Server::~Server()
{
  close(client_socket);
}
