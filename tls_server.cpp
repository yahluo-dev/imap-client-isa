#include <openssl/ssl.h>
#include <openssl/err.h>
#include <iostream>
#include <stdexcept>
#include "tls_server.hpp"
#include "tls_receiver.hpp"

SSL *TLSServer::get_ssl()
{
  return ssl;
}

void TLSServer::create_ssl_context()
{
  const SSL_METHOD *method = SSLv23_client_method();
  ctx = SSL_CTX_new(method);

  if (!ctx)
  {
    #ifndef NDEBUG
    ERR_print_errors_fp(stderr);
    #endif
    throw std::runtime_error("SSL context creation failed!");
  }


  if (!cert_file.empty())
  {
    logger.debug_log("Using certificate " + cert_file);
  }
  logger.debug_log("Using certificate directory " + cert_dir);

  const char *cert_file_c_string = cert_file.empty() ? nullptr : cert_file.c_str();
  const char *cert_dir_c_string = cert_dir.empty() ? nullptr : cert_dir.c_str();

  if (SSL_CTX_load_verify_locations(ctx, cert_file_c_string, cert_dir_c_string) != 1)
  {
    #ifndef NDEBUG
    ERR_print_errors_fp(stderr);
    #endif
    SSL_CTX_free(ctx);
    throw std::runtime_error("Error loading CA certificate file/directory.");
  }

  SSL_CTX_set_verify(ctx, SSL_VERIFY_PEER | SSL_VERIFY_FAIL_IF_NO_PEER_CERT, nullptr);
}

void TLSServer::send(std::unique_ptr<Command> command)
{
  std::string to_send = command->make_tcp();
  SSL_write(ssl, to_send.c_str(), to_send.size());
}

void TLSServer::receive(Session &session)
{
  receiving_thread = std::thread([this, &session]() {
    this->receiver->receive(session);
  });
}

TLSServer::TLSServer(const std::string hostname, const std::string port, const std::string _cert_file, const std::string _cert_dir)
  : Server(hostname, port), cert_dir(_cert_dir), cert_file(_cert_file) // NOTE: Careful with argument order
{
  SSL_library_init();
  SSL_load_error_strings();
  OpenSSL_add_ssl_algorithms();

  create_ssl_context();

  ssl = SSL_new(ctx);
  SSL_set_fd(ssl, client_socket);

  if (SSL_connect(ssl) <= 0)
  {
    #ifndef NDEBUG
    ERR_print_errors_fp(stderr);
    #endif
    throw std::runtime_error("Failed to initialize SSL!");
  }
  else
  {
    logger.debug_log("Connected with " + std::string(SSL_get_cipher(ssl)) + " encryption.");
  }

  receiver = std::make_unique<TLSReceiver>(ssl);
}

TLSServer::~TLSServer()
{
  SSL_CTX_free(ctx);
  EVP_cleanup();
}
