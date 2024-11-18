/**
 * @file tls_receiver.cpp
 * @brief A subclass of Receiver supporting TLS
 * @author Iaroslav Vasilevskii (xvasil10)
 */
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
    stopped = true; // Connection terminated
  return std::string(buffer, bytes_recvd);
}
