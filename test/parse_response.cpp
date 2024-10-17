#include <gtest/gtest.h>
#include "../response_parser.hpp"



TEST(ResponseParserParseTests, LoginNoError)
{
  std::string test_data = "A001 OK LOGIN completed.\r\n";

  //                response
  //              response-done
  //             response-tagged
  // tag                 SP resp-cond-state CRLF
  //                        OK SP resp-text
  //                               text

  ResponseParser parser = ResponseParser(test_data);

  ASSERT_NO_THROW(parser.parse());
}

TEST(ResponseParserParseTests, SelectResponseParseCorrect)
{
  std::vector<std::unique_ptr<Response>> expected;

  expected.push_back(std::make_unique<SingleNumberResponse>(ResponseType::EXISTS, 5));
  expected.push_back(std::make_unique<SingleNumberResponse>(ResponseType::RECENT, 0));
  expected.push_back(std::make_unique<StatusResponse>(ResponseType::OK, "", "[UNSEEN 2] Message 2 is the first unseen message"));
  expected.push_back(std::make_unique<StatusResponse>(ResponseType::OK, "A003", "[READ-WRITE] SELECT completed."));

  std::vector<std::string> test_data = {"* 5 EXISTS\r\n",
                          "* 0 RECENT\r\n",
                          "* OK [UNSEEN 2] Message 2 is the first unseen message\r\n",
                          "A003 OK [READ-WRITE] SELECT completed.\r\n"};

  std::shared_ptr<Response> result;

  ResponseParser parser = ResponseParser(test_data[0]);
  ASSERT_NO_THROW((result = parser.parse()));
  ASSERT_EQ(expected[0]->get_tag(), result->get_tag());

  parser = ResponseParser(test_data[1]);
  ASSERT_NO_THROW((result = parser.parse()));
  ASSERT_EQ(expected[1]->get_tag(), result->get_tag());

  parser = ResponseParser(test_data[2]);
  ASSERT_NO_THROW((result = parser.parse()));
  ASSERT_EQ(expected[2]->get_tag(), result->get_tag());

  parser = ResponseParser(test_data[3]);
  ASSERT_NO_THROW((result = parser.parse()));
  ASSERT_EQ(expected[3]->get_tag(), result->get_tag());
}


TEST(ResponseParserParseTests, GreetingParseCorrect)
{
  std::unique_ptr<Response> expected = std::make_unique<StatusResponse>(ResponseType::OK, "", "[CAPABILITY IMAP4rev1 SASL-IR LOGIN-REFERRALS ID ENABLE IDLE LITERAL+ STARTTLS AUTH=PLAIN AUTH=LOGIN] Dovecot ready.");
  std::string test_data = "* OK [CAPABILITY IMAP4rev1 SASL-IR LOGIN-REFERRALS ID ENABLE IDLE LITERAL+ STARTTLS AUTH=PLAIN AUTH=LOGIN] Dovecot ready.\r\n";

  ResponseParser parser = ResponseParser(test_data);

  std::shared_ptr<Response> result;

  ASSERT_NO_THROW((result = parser.parse()));

  ASSERT_EQ(expected->get_tag(), result->get_tag());
  ASSERT_EQ(expected->get_type(), result->get_type());
  ASSERT_EQ(expected->get_text(), result->get_text());
}

TEST(ResponseParserParseTests, LoginParseCorrect)
{
  std::unique_ptr<Response> expected = std::make_unique<StatusResponse>(ResponseType::OK, "A001", "LOGIN completed.");
  std::string test_data = "A001 OK LOGIN completed.\r\n";

  ResponseParser parser = ResponseParser(test_data);

  std::shared_ptr<Response> result;

  ASSERT_NO_THROW((result = parser.parse()));

  ASSERT_EQ(expected->get_tag(), result->get_tag());
  ASSERT_EQ(expected->get_type(), result->get_type());
  ASSERT_EQ(expected->get_text(), result->get_text());
}

TEST(ResponseParserParseTests, LoginParseCorrect2)
{
  std::unique_ptr<Response> expected = std::make_unique<StatusResponse>(ResponseType::OK, "a001", "[CAPABILITY IMAP4rev1 SASL-IR LOGIN-REFERRALS ID ENABLE IDLE SORT SORT=DISPLAY THREAD=REFERENCES THREAD=REFS THREAD=ORDEREDSUBJECT MULTIAPPEND URL-PARTIAL CATENATE UNSELECT CHILDREN NAMESPACE UIDPLUS LIST-EXTENDED I18NLEVEL=1 CONDSTORE QRESYNC ESEARCH ESORT SEARCHRES WITHIN CONTEXT=SEARCH LIST-STATUS BINARY MOVE SNIPPET=FUZZY PREVIEW=FUZZY PREVIEW STATUS=SIZE SAVEDATE LITERAL+ NOTIFY] Logged in");
  std::string test_data = "a001 OK [CAPABILITY IMAP4rev1 SASL-IR LOGIN-REFERRALS ID ENABLE IDLE SORT SORT=DISPLAY THREAD=REFERENCES THREAD=REFS THREAD=ORDEREDSUBJECT MULTIAPPEND URL-PARTIAL CATENATE UNSELECT CHILDREN NAMESPACE UIDPLUS LIST-EXTENDED I18NLEVEL=1 CONDSTORE QRESYNC ESEARCH ESORT SEARCHRES WITHIN CONTEXT=SEARCH LIST-STATUS BINARY MOVE SNIPPET=FUZZY PREVIEW=FUZZY PREVIEW STATUS=SIZE SAVEDATE LITERAL+ NOTIFY] Logged in\r\n";

  ResponseParser parser = ResponseParser(test_data);

  std::unique_ptr<Response> result;

  ASSERT_NO_THROW((result = parser.parse()));

  ASSERT_EQ(expected->get_tag(), result->get_tag());
  ASSERT_EQ(expected->get_type(), result->get_type());
  ASSERT_EQ(expected->get_text(), result->get_text());
}

