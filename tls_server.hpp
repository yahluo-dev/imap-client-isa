#ifndef TLS_SERVER_H_
#define TLS_SERVER_H_

#include <openssl/ssl.h>
#include "server.hpp"

class TLSServer : public Server
{
  private:
  void create_ssl_context();
  SSL *ssl;
  SSL_CTX *ctx;
  std::string cert_dir;
  std::string cert_file;
  virtual std::string receive_inner();
  public:
  TLSServer(const std::string hostname, const std::string port, const std::string _cert_file, const std::string _cert_dir);
  ~TLSServer();
  virtual void send(std::unique_ptr<Command> command);
};

#endif // TLS_SERVER_H_
