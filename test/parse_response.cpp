#include <gtest/gtest.h>
#include "../response_parser.hpp"



TEST(ResponseParserParseTests, LoginNoError)
{
  std::string test_data = "A001 OK LOGIN completed.\r\n";

  // response
  // response-done
  // response-tagged
  // tag                 SP resp-cond-state CRLF
  //                        OK SP resp-text
  //                               text

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

  ASSERT_NO_THROW(parser.parse());

  parser = ResponseParser(test_data[1]);

  ASSERT_NO_THROW(parser.parse());

  parser = ResponseParser(test_data[2]);

  ASSERT_NO_THROW(parser.parse());

  parser = ResponseParser(test_data[3]);

  ASSERT_NO_THROW(parser.parse());
}


TEST(ResponseParserParseTests, GreetingNoError)
{
  std::string test_data = "* OK [CAPABILITY IMAP4rev1 SASL-IR LOGIN-REFERRALS ID ENABLE IDLE LITERAL+ STARTTLS AUTH=PLAIN AUTH=LOGIN] Dovecot ready.\r\n";

  ResponseParser parser = ResponseParser(test_data);

  EXPECT_TRUE(parser.parse());
}

TEST(ResponseParserParseTests, LoginParseCorrect)
{
  std::unique_ptr<Response> expected = std::make_unique<StatusResponse>(ResponseType::OK, "A001", "OK LOGIN completed.");
  std::string test_data = "A001 OK LOGIN completed.\r\n";

  ResponseParser parser = ResponseParser(test_data);

  std::shared_ptr<Response> result;

  ASSERT_NO_THROW((result = parser.parse()));

  ASSERT_EQ(expected->get_tag(), result->get_tag());
  ASSERT_EQ(expected->get_type(), result->get_type());
}
