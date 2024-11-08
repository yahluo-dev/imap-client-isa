#include "tls_receiver.hpp"
#include <stdexcept>

SSL *TLSReceiver::ssl;

TLSReceiver::TLSReceiver(SSL *_ssl)
{
  ssl = _ssl;
}

std::string TLSReceiver::receive_inner()
{
  ssize_t bytes_recvd;
  char buffer[RECVMESSAGE_MAXLEN] = {};
  if (-1 == (bytes_recvd = SSL_read(ssl, &buffer, RECVMESSAGE_MAXLEN)))
  {
    if (errno == EWOULDBLOCK || errno == EAGAIN)
    {
      return "";
    }
    throw std::runtime_error("recv() failed");
  }
  else if (bytes_recvd == 0)
    throw std::logic_error("Connection already terminated!");
  return std::string(buffer, bytes_recvd);
}
