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
public:
  Receiver();
  Receiver(int sock);
  static volatile bool stopped;
  void receive(Session& session);
};

#endif // RECEIVER_H_
