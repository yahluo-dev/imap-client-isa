#include <gtest/gtest.h>
#include "../response_parser.hpp"



TEST(ResponseParserParseTests, LoginNoError)
{
  // TextResponse expected = TextResponse(RESPONSE_OK, "A001", "LOGIN completed.");

  std::string test_data = "A001 OK LOGIN completed.\r\n";

  ResponseParser parser = ResponseParser(test_data);

  EXPECT_TRUE(parser.parse());
}

TEST(ResponseParserParseTests, SelectResponseNoError)
{
  std::vector<std::string> test_data = {"* 5 EXISTS\r\n",
                          "* 0 RECENT\r\n",
                          "* OK [UNSEEN 2] Message 2 is the first unseen message\r\n",
                          "A003 OK [READ-WRITE] SELECT completed.\r\n"};

  ResponseParser parser = ResponseParser(test_data[0]);

  EXPECT_TRUE(parser.parse());

  parser = ResponseParser(test_data[1]);

  EXPECT_TRUE(parser.parse());

  parser = ResponseParser(test_data[2]);

  EXPECT_TRUE(parser.parse());

  parser = ResponseParser(test_data[3]);

  EXPECT_TRUE(parser.parse());
}
