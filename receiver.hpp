#ifndef RECEIVER_H_
#define RECEIVER_H_

#include <string>

#include "session.hpp"

#include "server.hpp"

class Session;

class Receiver
{
protected:
  static std::string received_data;
  static constexpr size_t RECVMESSAGE_MAXLEN = 4096;
  static int client_socket;
  virtual std::string receive_inner();
public:
  Receiver();
  Receiver(Server &_server);
  static volatile bool stopped;
  void receive(Session& session);
};

#endif // RECEIVER_H_
