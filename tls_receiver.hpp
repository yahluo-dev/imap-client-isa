#ifndef TLS_RECEIVER_H_
#define TLS_RECEIVER_H_

#include <openssl/ssl.h>
#include "tls_server.hpp"
#include "receiver.hpp"

class TLSReceiver : public Receiver
{
private:
  static SSL *ssl;
public:
  TLSReceiver(TLSServer &tls_server);
  virtual std::string receive_inner();
};

#endif // TLS_RECEIVER_H_
