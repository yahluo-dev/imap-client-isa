#include "tcp_stream_buffer.hpp"

std::streambuf::int_type TCPStreamBuf::underflow()
{
  if (gptr() == egptr())
  {
    ssize_t bytes_read = recv(socket_fd, buffer, buffer_size, 0);
    if (bytes_read <= 0)
    {
      return traits_type::eof();
    }
    setg(buffer, buffer, buffer + bytes_read);
  }

  return traits_type::to_int_type(*gptr());
}

TCPStreamBuf::TCPStreamBuf(int fd) : socket_fd(fd)
{
  setg(buffer, buffer, buffer);
}

TCPStreamBuf::~TCPStreamBuf()
{
  if (socket_fd >= 0)
  {
    close(socket_fd);
  }
}
