#include <streambuf>
#include <istream>
#include <sys/socket.h>
#include <unistd.h>

#ifndef TCP_STREAM_BUFFER_H_
#define TCP_STREAM_BUFFER_H_

class TCPStreamBuf : public std::streambuf
{
  static constexpr size_t buffer_size = 1024;
  char buffer[buffer_size];
  int socket_fd;
  protected:
  int_type underflow() override;
  public:
  explicit TCPStreamBuf(int fd);
  ~TCPStreamBuf();
};

#endif // TCP_STREAM_BUFFER_H_
