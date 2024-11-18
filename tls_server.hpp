/**
 * @file tls_server.hpp
 * @brief Server subclass adding SSL/TLS initialization and management
 * @author Iaroslav Vasilevskii (xvasil10)
 */
#ifndef TLS_SERVER_H_
#define TLS_SERVER_H_

#include <openssl/ssl.h>
#include "server.hpp"
#include "tls_receiver.hpp"

class TLSServer : public Server
{
  private:
  void create_ssl_context();
  SSL *ssl;
  SSL_CTX *ctx;
  std::string cert_dir;
  std::string cert_file;
  std::unique_ptr<TLSReceiver> receiver;
  public:
  TLSServer(const std::string hostname, const std::string port, const std::string _cert_file, const std::string _cert_dir);
  ~TLSServer();
  SSL *get_ssl();
  virtual void send(std::unique_ptr<Command> command);
  virtual void receive(Session &session);
};

#endif // TLS_SERVER_H_
