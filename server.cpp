/**
 * @file server.cpp
 * @brief Class initializing the socket and managing message sending
 * @author Iaroslav Vasilevskii (xvasil10)
 */
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <fcntl.h>
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
#define CONNECT_TIMEOUT_S 5

// https://stackoverflow.com/a/2597669
void Server::connect_with_timeout(const std::string hostname, const std::string port)
{
  int rv;
  struct addrinfo hints = {0};
  struct addrinfo *p;
  struct addrinfo *server_addrinfo;
  struct timeval tv;
  struct timeval connect_tv;
  fd_set fdset;

  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;

  // Resolve hostname
  if (0 != (rv = getaddrinfo(hostname.c_str(), port.c_str(), &hints, &server_addrinfo)))
  {
    throw std::runtime_error(gai_strerror(rv));
  }

  // Create socket based on addrinfo
  for(p = server_addrinfo; p != NULL; p = p->ai_next) {
    if ((client_socket = socket(p->ai_family, p->ai_socktype,
                                p->ai_protocol)) == -1) {
      throw std::runtime_error(strerror(errno));
      continue;
    }

    long arg;
    // Set non-blocking
    if ((arg = fcntl(client_socket, F_GETFL, NULL)) < 0)
    {
      throw std::runtime_error("fcntl(..., F_GETFL) -- " + std::string(strerror(errno)));
    }
    arg |= O_NONBLOCK;
    if (fcntl(client_socket, F_SETFL, arg) < 0)
    {
      throw std::runtime_error("fcntl(..., F_SETFL) -- " + std::string(strerror(errno)));
    }

    // Connect with non-blocking socket
    if (connect(client_socket, p->ai_addr, p->ai_addrlen) < 0) {
      if (errno == EINPROGRESS)
      {
        logger.debug_log("Socket: EINPROGRESS");
        do
        {
          connect_tv.tv_sec = CONNECT_TIMEOUT_S;
          connect_tv.tv_usec = 0;
          FD_ZERO(&fdset);;
          FD_SET(client_socket, &fdset);
          int res;
          res = select(client_socket+1, NULL, &fdset, NULL, &connect_tv);
          if (res < 0 && errno != EINTR)
          {
            throw std::runtime_error("Error connecting socket -- " + std::string(strerror(errno)));
          }
          else if (res > 0)
          {
            unsigned int lon;
            int valopt;
            lon = sizeof(int);
            if (getsockopt(client_socket, SOL_SOCKET, SO_ERROR, (void*)(&valopt), &lon) < 0)
            {
              throw std::runtime_error("Error in getsockopt() -- " + std::string(strerror(errno)));
            }
            if (valopt)
            {
              throw std::runtime_error("Error in delayed connection -- " + std::string(strerror(errno)));
            }
            break;
          }
          else
          {
            throw std::runtime_error("Timed out waiting for response.");
          }
        } while(true);
      }
      else
      {
        close(client_socket);
        throw std::runtime_error("Error connecting -- " + std::string(strerror(errno)));
        continue;
      }
    }
    // Make the socket blocking again
    if ((arg = fcntl(client_socket, F_GETFL, NULL)) < 0)
    {
      throw std::runtime_error("fcntl(..., F_GETFL) -- " + std::string(strerror(errno)));
    }
    arg &= (~O_NONBLOCK);
    if (fcntl(client_socket, F_SETFL, arg) < 0)
    {
      throw std::runtime_error("fcntl(..., F_SETFL) -- " + std::string(strerror(errno)));
    }
  }

  // This ensures that the receiver eventually leaves
  // uninterruptible sleep upon calling recv()
  tv.tv_sec = TIMEOUT_S;
  tv.tv_usec = 0;
  setsockopt(client_socket, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));

  freeaddrinfo(server_addrinfo);

}

Server::Server(const std::string hostname, const std::string port)
  : logger(std::cerr)
{
  connect_with_timeout(hostname, port);
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
  // The thread may not be running if a derived class constructor threw (which it can)!
  if (receiving_thread.joinable())
  {
    receiving_thread.join();
  }
  close(client_socket);
}
