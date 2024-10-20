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

TEST(ResponseParserParseTests, SelectResponseParseCorrect)
{
  std::vector<std::unique_ptr<Response>> expected;

  expected.push_back(std::make_unique<SingleNumberResponse>(ResponseType::EXISTS, 5));
  expected.push_back(std::make_unique<SingleNumberResponse>(ResponseType::RECENT, 0));
  expected.push_back(std::make_unique<StatusResponse>(ResponseType::OK, "", "[UNSEEN 2] Message 2 is the first unseen message"));
  expected.push_back(std::make_unique<StatusResponse>(ResponseType::OK, "A003", "[READ-WRITE] SELECT completed."));

  std::vector<std::string> test_data = {"* 5 EXISTS\r\n",
                          "* 0 RECENT\r\n",
                          "* OK [UNSEEN 2] Message 2 is the first unseen message\r\n", // FIXME: This is not valid per RFC. FLAGS Missing
                          "A003 OK [READ-WRITE] SELECT completed.\r\n"};

  std::unique_ptr<Response> result;

  std::unique_ptr<ResponseParser> parser = std::make_unique<ResponseParser>(test_data[0]);
  ASSERT_NO_THROW((result = parser->parse()));
  ASSERT_EQ(expected[0]->get_tag(), result->get_tag());

  parser = std::make_unique<ResponseParser>(test_data[1]);
  ASSERT_NO_THROW((result = parser->parse()));
  ASSERT_EQ(expected[1]->get_tag(), result->get_tag());

  parser = std::make_unique<ResponseParser>(test_data[2]);
  ASSERT_NO_THROW((result = parser->parse()));
  ASSERT_EQ(expected[2]->get_tag(), result->get_tag());

  parser = std::make_unique<ResponseParser>(test_data[3]);
  ASSERT_NO_THROW((result = parser->parse()));
  ASSERT_EQ(expected[3]->get_tag(), result->get_tag());
}

TEST(ResponseParserParseTests, SelectResponseParseCorrect2)
{
  std::vector<std::unique_ptr<Response>> expected;

  expected.push_back(std::make_unique<StatusResponse>(ResponseType::OK, "", "[CLOSED] Previous mailbox closed."));
  expected.push_back(std::make_unique<FlagsResponse>());
  expected.push_back(std::make_unique<StatusResponse>(ResponseType::OK, "", "[PERMANENTFLAGS (\\Answered \\Flagged \\Deleted \\Seen \\Draft \\*)] Flags permitted."));
  expected.push_back(std::make_unique<SingleNumberResponse>(ResponseType::EXISTS, 5));
  expected.push_back(std::make_unique<SingleNumberResponse>(ResponseType::RECENT, 0));
  expected.push_back(std::make_unique<StatusResponse>(ResponseType::OK, "", "[UNSEEN 5] First unseen."));
  expected.push_back(std::make_unique<StatusResponse>(ResponseType::OK, "", "[UIDVALIDITY 1727693727] UIDs valid"));
  expected.push_back(std::make_unique<StatusResponse>(ResponseType::OK, "", "[UIDNEXT 6] Predicted next UID"));
  expected.push_back(std::make_unique<StatusResponse>(ResponseType::OK, "4", "[READ-WRITE] Select completed (0.014 + 0.000 + 0.013 secs)."));

  std::vector<std::string> test_data = {"* OK [CLOSED] Previous mailbox closed.\r\n",
                                        "* FLAGS (\\Answered \\Flagged \\Deleted \\Seen \\Draft)\r\n",
                                        "* OK [PERMANENTFLAGS (\\Answered \\Flagged \\Deleted \\Seen \\Draft \\*)] Flags permitted.\r\n",
                                        "* 5 EXISTS\r\n",
                                        "* 0 RECENT\r\n",
                                        "* OK [UNSEEN 5] First unseen.\r\n",
                                        "* OK [UIDVALIDITY 1727693727] UIDs valid\r\n",
                                        "* OK [UIDNEXT 6] Predicted next UID\r\n",
                                        "4 OK [READ-WRITE] Select completed (0.014 + 0.000 + 0.013 secs).\r\n"};

  std::unique_ptr<Response> result;

  std::unique_ptr<ResponseParser> parser = std::make_unique<ResponseParser>(test_data[0]);
  ASSERT_NO_THROW((result = parser->parse()));
  ASSERT_EQ(expected[0]->get_tag(), result->get_tag());

  parser = std::make_unique<ResponseParser>(test_data[1]);
  ASSERT_NO_THROW((result = parser->parse()));
  ASSERT_EQ(expected[1]->get_tag(), result->get_tag());

  parser = std::make_unique<ResponseParser>(test_data[2]);
  ASSERT_NO_THROW((result = parser->parse()));
  ASSERT_EQ(expected[2]->get_tag(), result->get_tag());

  parser = std::make_unique<ResponseParser>(test_data[3]);
  ASSERT_NO_THROW((result = parser->parse()));
  ASSERT_EQ(expected[3]->get_tag(), result->get_tag());

  parser = std::make_unique<ResponseParser>(test_data[4]);
  ASSERT_NO_THROW((result = parser->parse()));
  ASSERT_EQ(expected[4]->get_tag(), result->get_tag());

  parser = std::make_unique<ResponseParser>(test_data[5]);
  ASSERT_NO_THROW((result = parser->parse()));
  ASSERT_EQ(expected[5]->get_tag(), result->get_tag());

  parser = std::make_unique<ResponseParser>(test_data[6]);
  ASSERT_NO_THROW((result = parser->parse()));
  ASSERT_EQ(expected[6]->get_tag(), result->get_tag());

  parser = std::make_unique<ResponseParser>(test_data[7]);
  ASSERT_NO_THROW((result = parser->parse()));
  ASSERT_EQ(expected[7]->get_tag(), result->get_tag());

  parser = std::make_unique<ResponseParser>(test_data[8]);
  ASSERT_NO_THROW((result = parser->parse()));
  ASSERT_EQ(expected[8]->get_tag(), result->get_tag());
}
