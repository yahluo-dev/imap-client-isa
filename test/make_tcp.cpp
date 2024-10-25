#include <gtest/gtest.h>
#include <stdint.h>
#include <vector>
#include "../command.hpp"

TEST(CommandMakeTcpTests, Login)
{
  std::string expected = std::string("a001 LOGIN username password\r\n");
  LoginCommand command = LoginCommand("a001", "username", "password");

  std::string result = command.make_tcp();

  EXPECT_EQ(result, expected);
}

TEST(CommandMakeTcpTests, Select)
{
  std::string expected = std::string("a002 SELECT INBOX\r\n");
  SelectCommand command = SelectCommand("a002", "INBOX");

  std::string result = command.make_tcp();

  EXPECT_EQ(result, expected);
}

TEST(CommandMakeTcpTests, Search)
{
  std::string expected = std::string("a003 SEARCH ALL\r\n");
  SearchCommand command = SearchCommand("a003", "ALL");

  std::string result = command.make_tcp();

  EXPECT_EQ(result, expected);
}

TEST(CommandMakeTcpTests, Fetch)
{
  std::string expected = std::string("a004 FETCH 1,2,3,5 BODY[]\r\n");
  std::vector<uint32_t> seq_numbers = {1,2,3,5};
  FetchCommand command = FetchCommand("a004", seq_numbers, "BODY[]");

  std::string result = command.make_tcp();

  EXPECT_EQ(result, expected);
}
