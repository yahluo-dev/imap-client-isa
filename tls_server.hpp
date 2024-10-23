#ifndef TLS_SERVER_H_
#define TLS_SERVER_H_

#include <openssl/ssl.h>
#include "server.hpp"

class TLSServer : public Server
{
  private:
  SSL_CTX *create_ssl_context();
  SSL *ssl;
  SSL_CTX *ctx;
  public:
  TLSServer(const std::string hostname, const std::string port);
  ~TLSServer();
  void send(std::unique_ptr<Command> command);
};

#endif // TLS_SERVER_H_
