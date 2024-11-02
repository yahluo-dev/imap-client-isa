#ifndef TLS_RECEIVER_H_
#define TLS_RECEIVER_H_

#include <openssl/ssl.h>
#include "tls_server.hpp"
#include "receiver.hpp"

class TLSReceiver : public Receiver
{
protected:
  static SSL *ssl;
  virtual std::string receive_inner();
public:
  TLSReceiver(TLSServer &tls_server);
};

#endif // TLS_RECEIVER_H_
