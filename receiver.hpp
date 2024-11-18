/**
 * @file receiver.hpp
 * @brief Class implementing asynchronous receipt of messages
 * @author Iaroslav Vasilevskii (xvasil10)
 */
#ifndef RECEIVER_H_
#define RECEIVER_H_

#include <string>

class Session;

class Receiver
{
protected:
  static std::string received_data;
  static constexpr size_t RECVMESSAGE_MAXLEN = 4096;
  static int client_socket;
  virtual std::string receive_inner();
  static volatile bool stopped;
public:
  Receiver();
  Receiver(int sock);
  void receive(Session& session);
  void stop();
};

#endif // RECEIVER_H_
