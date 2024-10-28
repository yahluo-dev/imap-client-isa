#ifndef RECEIVER_H_
#define RECEIVER_H_

#include <string>

#include "session.hpp"

class Session;

class Receiver
{
private:
  static std::string received_data;
public:
  static volatile bool stopped;
  static std::string receive_inner(int client_socket);
  static void receive(Session& session, int client_socket);
};

#endif // RECEIVER_H_
