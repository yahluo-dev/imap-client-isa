#include <openssl/ssl.h>
#include <openssl/err.h>
#include <iostream>
#include "tls_server.hpp"


SSL_CTX *TLSServer::create_ssl_context()
{
  const SSL_METHOD *method = SSLv23_client_method();
  SSL_CTX *ctx = SSL_CTX_new(method);

  if (!ctx)
  {
    std::cerr << "SSL context creation failed!" << std::endl;
    ERR_print_errors_fp(stderr);
    exit(EXIT_FAILURE);
  }

  return ctx;
}

void TLSServer::send(std::unique_ptr<Command> command)
{
  std::string to_send = command->make_tcp();
  SSL_write(ssl, to_send.c_str(), to_send.size());
}

TLSServer::TLSServer(const std::string hostname, const std::string port) : Server(hostname, port)
{
  std::cout << "TLSServer constructor called" << std::endl;
  SSL_load_error_strings();
  OpenSSL_add_ssl_algorithms();

  ctx = create_ssl_context();

  SSL *ssl = SSL_new(ctx);
  SSL_set_fd(ssl, client_socket);

  if (SSL_connect(ssl) <= 0)
  {
    std::cerr << "SSL failed!" << std::endl;
    ERR_print_errors_fp(stderr);
  }
  else
  {
    std::cout << "Connected with " << SSL_get_cipher(ssl) << " encryption" << std::endl;
  }
}

TLSServer::~TLSServer()
{
  SSL_CTX_free(ctx);
  EVP_cleanup();
}
