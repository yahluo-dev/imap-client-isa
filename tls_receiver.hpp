/**
 * @file tls_receiver.hpp
 * @brief A subclass of Receiver supporting TLS
 * @author Iaroslav Vasilevskii (xvasil10)
 */
#ifndef TLS_RECEIVER_H_
#define TLS_RECEIVER_H_

#include <openssl/ssl.h>
#include "receiver.hpp"

class TLSReceiver : public Receiver
{
protected:
  static SSL *ssl;
  virtual std::string receive_inner() override;
public:
  TLSReceiver(SSL *ssl);
};

#endif // TLS_RECEIVER_H_
