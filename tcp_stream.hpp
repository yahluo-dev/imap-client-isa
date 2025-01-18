#ifndef TCP_STREAM_H_
#define TCP_STREAM_H_
#include "tcp_stream_buffer.hpp"

class TCPIStream : public std::istream
{
  TCPStreamBuf buf;
  public:
  explicit TCPIStream(int socket_fd) : std::istream(&buf), buf(socket_fd) {}
};

#endif // TCP_STREAM_H_
