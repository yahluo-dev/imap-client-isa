#include <openssl/ssl.h>
#include <openssl/err.h>
#include <iostream>
#include <format>
#include "tls_server.hpp"

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
    logger.error_log("SSL context creation failed!");
    ERR_print_errors_fp(stderr);
    exit(EXIT_FAILURE);
  }


  logger.debug_log("Using certificate " + cert_file);
  logger.debug_log("Using certificate directory " + cert_dir);

  const char *cert_file_c_string = cert_file.empty() ? nullptr : cert_file.c_str();
  const char *cert_dir_c_string = cert_dir.empty() ? nullptr : cert_dir.c_str();

  if (SSL_CTX_load_verify_locations(ctx, cert_file_c_string, cert_dir_c_string) != 1)
  {
    logger.error_log("Error loading CA certificate directory");
    ERR_print_errors_fp(stderr);
    SSL_CTX_free(ctx);
    exit(EXIT_FAILURE);
  }

  SSL_CTX_set_verify(ctx, SSL_VERIFY_PEER | SSL_VERIFY_FAIL_IF_NO_PEER_CERT, nullptr);
}

void TLSServer::send(std::unique_ptr<Command> command)
{
  std::string to_send = command->make_tcp();
  SSL_write(ssl, to_send.c_str(), to_send.size());
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
