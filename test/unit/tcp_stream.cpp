#include <gtest/gtest.h>
#include <sys/socket.h>
#include "../../tcp_stream.hpp"

TEST(TCPStream, ReceiveAll)
{
  const std::string test_data = "AAAABBBBCCCCDDDDEEEE";
  std::stringstream message;

  int sockets[2];
  assert(socketpair(AF_UNIX, SOCK_STREAM, 0, sockets) == 0);

  TCPIStream tcp_stream_buf(sockets[0]);

  write(sockets[1], test_data.c_str(), test_data.size());
  close(sockets[1]);

  std::string result((std::istreambuf_iterator<char>(tcp_stream_buf)),
                        std::istreambuf_iterator<char>());

  ASSERT_EQ(result, test_data);
  close(sockets[0]);
}

TEST(TCPStream, ReceiveOneByOne)
{
  const std::string test_data = "AAAABBBBCCCCDDDDEEEE";
  const std::string expected = "AAAABBBBCC";
  std::stringstream message;

  int sockets[2];
  assert(socketpair(AF_UNIX, SOCK_STREAM, 0, sockets) == 0);

  TCPIStream tcp_stream_buf(sockets[0]);

  write(sockets[1], test_data.c_str(), test_data.size());

  auto it = std::istreambuf_iterator<char>(tcp_stream_buf);
  auto end = std::istreambuf_iterator<char>();

  std::string result;
  result.reserve(10);

  for (int i = 0; i < 10 && it != end; i++, it++)
  {
    result.push_back(*it);
  }
  close(sockets[1]);

  ASSERT_EQ(result, expected);
  close(sockets[0]);
}
