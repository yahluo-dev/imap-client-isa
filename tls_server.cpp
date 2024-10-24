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

  const char * cert_file_c_string = cert_file.empty() ? nullptr : cert_file.c_str();
  if (cert_dir.empty())
  {
    throw std::runtime_error("cert_dir is an empty string!");
  }
  std::cout << "Using certificate " << cert_file << std::endl;
  std::cout << "Using certificate directory " << cert_dir << std::endl;

  if (SSL_CTX_load_verify_locations(ctx, cert_file_c_string, cert_dir.c_str()) != 1)
  {
    std::cerr << "Error loading CA certificate directory" << std::endl;
    ERR_print_errors_fp(stderr);
    SSL_CTX_free(ctx);
    exit(EXIT_FAILURE);
  }

  return ctx;
}

std::string TLSServer::receive_inner()
{
  ssize_t bytes_recvd;
  char buffer[RECVMESSAGE_MAXLEN] = {};
  std::cout << "TLSServer receive_inner called" << std::endl;
  if (-1 == (bytes_recvd = SSL_read(ssl, &buffer, RECVMESSAGE_MAXLEN)))
  {
    throw std::runtime_error("recv() failed");
  }
  if (bytes_recvd == 0)
  {
    ERR_print_errors_fp(stderr);
    throw std::logic_error("Connection already terminated!");
  }
  return std::string(buffer, bytes_recvd);
}

void TLSServer::send(std::unique_ptr<Command> command)
{
  std::string to_send = command->make_tcp();
  SSL_write(ssl, to_send.c_str(), to_send.size());
}

TLSServer::TLSServer(const std::string hostname, const std::string port, const std::string _cert_file, const std::string _cert_dir)
  : Server(hostname, port), cert_dir(_cert_dir), cert_file(_cert_file) // NOTE: Careful with argument order
{
  std::cout << "TLSServer constructor called" << std::endl;
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
