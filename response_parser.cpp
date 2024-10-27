#include <cctype>
#include <iostream>
#include <format>
#include <regex>
#include <stdio.h>
#include <assert.h>
#include "response_parser.hpp"
#include "logger.hpp"

#define PARSE_FAIL {                                                    \
    logger.parser_debug_log(pos_stack.size(), curr_pos, __func__, "Failed."); \
    restore_pos();return false;                                         \
}

#define PARSE_SUCCESS {                                     \
    logger.parser_debug_log(pos_stack.size(), curr_pos, __func__, "Succeeded."); \
    pop_pos();                                                          \
    return true;                                                        \
}

#define EXPECT_MATCH(s) {\
  if (!match(s))\
  {\
    restore_pos();\
    return false;\
  }\
}

void ResponseParser::save_pos()
{
  pos_stack.push(curr_pos);
}

void ResponseParser::restore_pos()
{
  curr_pos = pos_stack.top();
  pos_stack.pop();
}

void ResponseParser::pop_pos()
{
  pos_stack.pop();
}

bool ResponseParser::match(std::string expected)
{
  if (data.substr(curr_pos, expected.size()) == expected)
  {
    curr_pos += expected.size();
    return true;
  }
  return false;
}

bool ResponseParser::regex_match(std::regex expected)
{
  std::smatch match;
  std::string substring = data.substr(curr_pos);
  std::regex_search(substring, match, expected);
  if (match.size() > 0)
  {
    curr_pos += match[0].str().size();
    return true;
  }
  return false;
}

bool ResponseParser::match_crlf()
{
  save_pos();
  if (!match("\r\n"))
  {
    restore_pos();
    return false;
  }
  pop_pos();
  return true;
}

bool ResponseParser::parse_number(int &number)
{
  save_pos();
  bool success = false;
  number = 0;
  while (std::isdigit(data[curr_pos]))
  {
    number *= 10;
    number += data[curr_pos]-'0';
    curr_pos += 1;
    success = true;
  }
  if (success)
    PARSE_SUCCESS
  else
    PARSE_FAIL
  return success;
}

bool ResponseParser::parse_tag()
{
  bool success = false;
  while (std::isalnum(data[curr_pos]))
  {
    curr_pos += 1;
    success = true;
  }
  return success;
}

// TEXT-CHAR       = <any CHAR except CR and LF>
// CHAR            = Any ASCII character, excluding NUL (ABNF)
bool ResponseParser::parse_text_char()
{
  save_pos();

  if (data[curr_pos] == '\r' || data[curr_pos] == '\n')
  {
    restore_pos();
    return false;
  }
  else
  {
    curr_pos += 1;
    pop_pos();
    return true;
  }
}

// text = 1*TEXT-CHAR
bool ResponseParser::parse_text()
{
  save_pos();
  bool success = false;

  if (parse_text_char())
    success = true;

  while(parse_text_char());

  if (success)
  {
    pop_pos();
    return true;
  }
  else
  {
    restore_pos();
    return false;
  }
}

// resp-text-code  = "ALERT" /
//                   "BADCHARSET" [SP "(" astring *(SP astring) ")" ] /
//                   capability-data / "PARSE" /
//                   "PERMANENTFLAGS" SP "("
//                   [flag-perm *(SP flag-perm)] ")" /
//                   "READ-ONLY" / "READ-WRITE" / "TRYCREATE" /
//                   "UIDNEXT" SP nz-number / "UIDVALIDITY" SP nz-number /
//                   "UNSEEN" SP nz-number /
//                   atom [SP 1*<any TEXT-CHAR except "]">]
bool ResponseParser::parse_resp_text_code()
{
  save_pos();
  if (match("ALERT"))
    PARSE_SUCCESS
  else if (match("UNSEEN"))
  {
    int number;
    EXPECT_MATCH(" ");
    if (!parse_number(number))
      PARSE_FAIL
    PARSE_SUCCESS
  }
  else if (match("READ-ONLY") || match("READ-WRITE"))
  {
    PARSE_SUCCESS
  }
  else // No idea what this is, so skip it.
  {
    while(curr_pos < data.size())
    {
      if(data[curr_pos] == ']')
        break;
      else
        curr_pos++;
    }
    if (curr_pos < data.size())
      if (data[curr_pos] == ']')
      {
        PARSE_SUCCESS
      }
  }
  PARSE_FAIL
  // TODO
}

// resp-text       = ["[" resp-text-code "]" SP] text
bool ResponseParser::parse_resp_text()
{
  save_pos();
  if (match("["))
  {
    parse_resp_text_code();
    if (!match("]"))
      PARSE_FAIL
    if (!match(" "))
      PARSE_FAIL
  }

  if (!parse_text())
  {
    restore_pos();
    return false;
  }
  pop_pos();
  return true;
}

// resp-cond-bye   = "BYE" SP resp-text
bool ResponseParser::parse_resp_cond_bye(std::unique_ptr<Response> &parsed_response)
{
  save_pos();
  if(!match("BYE"))
  {
    restore_pos();
    return false;
  }
  size_t text_start = curr_pos;
  if (!parse_resp_text())
  {
    restore_pos();
    return false;
  }
  std::string response_text = data.substr(text_start, curr_pos-text_start);
  parsed_response = std::make_unique<StatusResponse>(ResponseType::BYE, "", response_text);

  pop_pos();
  return true;
}

// resp-cond-state = ("OK" / "NO" / "BAD") SP resp-text
//                     ; Status condition
bool ResponseParser::parse_resp_cond_state(std::unique_ptr<Response> &parsed_response)
{
  save_pos();
  ResponseType response_type;
  std::string response_text;

  if (match("OK"))
  {
    response_type = ResponseType::OK;
  }
  else if (match("NO"))
  {
    response_type = ResponseType::NO;
  }
  else if (match("BAD"))
  {
    response_type = ResponseType::BAD;
  }
  else
  {
    restore_pos();
    return false;
  }

  if (!match(" "))
  {
    restore_pos();
    return false;
  }

  size_t text_start = curr_pos;
  if (!parse_resp_text())
  {
    restore_pos();
    return false;
  }
  response_text = data.substr(text_start, curr_pos-text_start);

  parsed_response = std::make_unique<StatusResponse>(response_type, "", response_text); // FIXME: Smart pointer with polymorphism

  pop_pos();
  return true;
}

// response-tagged = tag SP resp-cond-state CRLF
bool ResponseParser::parse_response_tagged(std::unique_ptr<Response> &parsed_response)
{
  save_pos();
  size_t tag_start = curr_pos;
  if (!parse_tag())
  {
    restore_pos();
    return false;
  }
  std::string tag = data.substr(tag_start, curr_pos-tag_start);

  if (!match(" "))
  {
    restore_pos();
    return false;
  }
  if (!parse_resp_cond_state(parsed_response))
  {
    restore_pos();
    return false;
  }
  if (!match_crlf())
    PARSE_FAIL

  parsed_response->set_tag(tag);
  pop_pos();
  return true;
}

// response-fatal  = "*" SP resp-cond-bye CRLF
//                     ; Server closes connection immediately
bool ResponseParser::parse_response_fatal(std::unique_ptr<Response> &parsed_response)
{
  save_pos();
  if (!match("*"))
  {
    restore_pos();
    return false;
  }
  if (!match(" "))
  {

    restore_pos();
    return false;
  }
  std::string bye_text;
  if (!parse_resp_cond_bye(parsed_response))
  {
    restore_pos();
    return false;
  }

  pop_pos();
  return true;
}

// response-done   = response-tagged / response-fatal
bool ResponseParser::parse_response_done(std::unique_ptr<Response> &parsed_response)
{
  save_pos();
  if (parse_response_tagged(parsed_response))
  {
    pop_pos();
    return true;
  }
  else if (parse_response_fatal(parsed_response))
  {
    pop_pos();
    return true;
  }
  restore_pos();
  return false;
}

bool ResponseParser::parse_continue_req()
{
  save_pos();

  if (!match("+"))
  {
    restore_pos();
    return false;
  }
  if (!match(" "))
  {
    restore_pos();
    return false;
  }
  if (!parse_resp_text())
  {
    restore_pos(); // no support for base64 for now
    return false;
  }
  if (!match_crlf())
  {
    restore_pos();
    return false;
  }

  pop_pos();
  return true;
}

bool ResponseParser::parse_flag_list()
{
  save_pos();
  EXPECT_MATCH("(");
  while(curr_pos < data.size())
  {
    if(data[curr_pos] == ')')
      break;
    else
      curr_pos++;
  }
  if (curr_pos >= data.size())
    throw std::runtime_error("Didn't understand the server's response!");
  EXPECT_MATCH(")");
  PARSE_SUCCESS
}

bool ResponseParser::parse_mailbox_list()
{
 // TODO
 return false;
}

bool ResponseParser::parse_message_id_list(std::vector<uint32_t> &numbers)
{
  save_pos();
  do
  {
    int number;
    if (!parse_number(number))
      PARSE_FAIL
    numbers.push_back(number);
  } while (match(" "));

  PARSE_SUCCESS
}

// quoted          = DQUOTE *QUOTED-CHAR DQUOTE
bool ResponseParser::parse_quoted()
{
  throw std::logic_error("Not implemented");
}

// astring         = 1*ASTRING-CHAR / string
bool ResponseParser::parse_astring()
{
  throw std::logic_error("Not implemented");
}

bool ResponseParser::parse_mailbox()
{
  if (regex_match(std::regex("^INBOX", std::regex_constants::icase)))
  {

  }
  else if (parse_tag()) // FIXME: parse_astring has to be here
  {

  }
  throw std::logic_error("Not implemented.");
}

// mailbox-data    =  "FLAGS" SP flag-list / "LIST" SP mailbox-list /
//                    "LSUB" SP mailbox-list / "SEARCH" *(SP nz-number) /
//                    "STATUS" SP mailbox SP "(" [status-att-list] ")" /
//                    number SP "EXISTS" / number SP "RECENT"
bool ResponseParser::parse_mailbox_data(std::unique_ptr<Response> &parsed_response)
{
  save_pos();
  int number = 0;
  if (match("FLAGS")) // "FLAGS" SP flag-list
  {
    EXPECT_MATCH(" ");
    if (!parse_flag_list())
    {
      restore_pos();
      return false; // Can this even happen for a valid string?
    }
    parsed_response = std::make_unique<FlagsResponse>();
    pop_pos();
    return true;
  }
  else if (match("LIST")) // "LIST" SP mailbox-list
  {
    EXPECT_MATCH(" ");
    if (!parse_mailbox_list())
    {
      restore_pos();
      return false;
    }
  }
  else if (match("LSUB")) // "LSUB" SP mailbox-list
  {
    EXPECT_MATCH(" ");
    if (!parse_mailbox_list())
    {
      restore_pos();
      return false;
    }
  }
  else if (match("SEARCH")) // "SEARCH" *(SP nz-number)
  {
    EXPECT_MATCH(" ");
    std::vector<uint32_t> ids;
    if (!parse_message_id_list(ids))
      PARSE_FAIL
    parsed_response = std::make_unique<SearchResponse>(ids);
  }
  else if (match("STATUS")) // "STATUS" SP mailbox SP "(" [status-att-list] ")"
  {
    EXPECT_MATCH(" ");
    if (!parse_mailbox())
      PARSE_FAIL
    EXPECT_MATCH(" ");
    EXPECT_MATCH("(");
    throw std::logic_error("Not implemented.");
    // while (parse_status-att-list()) // TODO
    // ;
    EXPECT_MATCH(")");
    PARSE_SUCCESS
  }
  else if (parse_number(number)) // number SP "EXISTS" / number SP "RECENT"
  {
    EXPECT_MATCH(" ");
    if (match("EXISTS"))
    {
      parsed_response = std::make_unique<SingleNumberResponse>(ResponseType::EXISTS, number);
      PARSE_SUCCESS
    }
    else if (match("RECENT"))
    {
      parsed_response = std::make_unique<SingleNumberResponse>(ResponseType::RECENT, number);
      PARSE_SUCCESS
    }
    else
    {
      PARSE_FAIL
    }
  }
  else
  {
    PARSE_FAIL
  }
  PARSE_SUCCESS
}

// msg-att-dynamic = "FLAGS" SP "(" [flag-fetch *(SP flag-fetch)] ")"
//                     ; MAY change for a message
bool ResponseParser::parse_msg_att_dynamic()
{
  save_pos();
  EXPECT_MATCH("FLAGS");
  EXPECT_MATCH(" ");
  EXPECT_MATCH("(");
  while(curr_pos < data.size()) // Skip
  {
    if(data[curr_pos] == ')')
      break;
    else
      curr_pos++;
  }
  if (curr_pos >= data.size())
    throw std::runtime_error("Unmatched bracket.");
  EXPECT_MATCH(")");

  PARSE_SUCCESS
}

bool ResponseParser::parse_section()
{
  save_pos();
  EXPECT_MATCH("[");
  while(curr_pos < data.size()) // Skip
  {
    if(data[curr_pos] == ']')
      break;
    else
      curr_pos++;
  }
  if (curr_pos >= data.size())
    throw std::runtime_error("Unmatched bracket.");
  EXPECT_MATCH("]");

  PARSE_SUCCESS
}

// msg-att-static  = "ENVELOPE" SP envelope / "INTERNALDATE" SP date-time /
//                   "RFC822" [".HEADER" / ".TEXT"] SP nstring /
//                   "RFC822.SIZE" SP number /
//                   "BODY" ["STRUCTURE"] SP body /
//                   "BODY" section ["<" number ">"] SP nstring /
//                   "UID" SP uniqueid
//                     ; MUST NOT change for a message
bool ResponseParser::parse_msg_att_static(std::string &message_contents)
{
  save_pos();
  if (match("ENVELOPE"))
    throw std::logic_error("Not implemented");
  else if (match("INTERNALDATE"))
    throw std::logic_error("Not implemented");
  else if (match("RFC822"))
    throw std::logic_error("Not implemented");
  else if (match("RFC822.SIZE"))
    throw std::logic_error("Not implemented");
  else if (match("BODY"))
  {
    if (match("STRUCTURE"))
    {
      throw std::logic_error("Not implemented");
    }
    else if (match(" "))
    {
      throw std::logic_error("Not implemented");
    }
    else if (parse_section())
    {
      if (match("<"))
      {
        throw std::logic_error("Not implemented");
      }
      EXPECT_MATCH(" ");

      if (!parse_nstring(message_contents))
      {
        PARSE_FAIL
      }
      PARSE_SUCCESS
    }
  }
  else if (match("UID"))
    throw std::logic_error("Not implemented");
  PARSE_FAIL
}

// literal         = "{" number "}" CRLF *CHAR8
//                     ; Number represents the number of CHAR8s
bool ResponseParser::parse_literal(std::string &parsed_literal)
{
  save_pos();
  int length = 0;
  EXPECT_MATCH("{");
  parse_number(length);
  EXPECT_MATCH("}");
  if (!match_crlf())
    PARSE_FAIL;
  parsed_literal = "";
  for (int i = 0; i < length; i++)
  {
    parsed_literal += data[curr_pos+i];
  }
  curr_pos += length;
  if (parsed_literal.size() < static_cast<size_t>(length))
  {
    throw std::runtime_error("Error: Literal string received from server shorter than expected!");
  }
  PARSE_SUCCESS
}


// string          = quoted / literal
bool ResponseParser::parse_string(std::string &parsed_string)
{
  save_pos();
  //if (parse_quoted())
  // else
  if (parse_literal(parsed_string))
  {
    PARSE_SUCCESS
  }

  PARSE_FAIL
}

// nstring         = string / nil
// nil             = "NIL"
bool ResponseParser::parse_nstring(std::string &parsed_nstring)
{
  save_pos();
  if (match("NIL"))
  {
    parsed_nstring = "";
  }
  else if (parse_string(parsed_nstring))
  {
    PARSE_SUCCESS
  }
  PARSE_FAIL
}

// msg-att         = "(" (msg-att-dynamic / msg-att-static)
//                    *(SP (msg-att-dynamic / msg-att-static)) ")"
bool ResponseParser::parse_msg_att(std::string &message_contents)
{
  save_pos();

  EXPECT_MATCH("(");
  if (parse_msg_att_dynamic()){}
  else if (parse_msg_att_static(message_contents)){}
  else
  {
    PARSE_FAIL
  }
  while(match(" "))
  {
    if (parse_msg_att_dynamic()){}
    else if (parse_msg_att_static(message_contents)){}
    else
    {
      break;
    }
  }
  EXPECT_MATCH(")");

  PARSE_SUCCESS
}

// message-data    = nz-number SP ("EXPUNGE" / ("FETCH" SP msg-att))
bool ResponseParser::parse_message_data(std::unique_ptr<Response> &parsed_response)
{
  save_pos();
  int number = 0;
  if (!parse_number(number))
  {
    PARSE_FAIL
  }
  EXPECT_MATCH(" ");

  if (match("EXPUNGE"))
    throw std::logic_error("Not implemented");
  else if (match("FETCH"))
  {
    std::string message_contents;
    EXPECT_MATCH(" ");
    if (!parse_msg_att(message_contents))
    {
      PARSE_FAIL
    }
    parsed_response = std::make_unique<FetchResponse>(message_contents);
  }
  else
  {
    PARSE_FAIL
  }
  PARSE_SUCCESS
}


// response-data   = "*" SP (resp-cond-state / resp-cond-bye /
//                   mailbox-data / message-data / capability-data) CRLF
bool ResponseParser::parse_response_data(std::unique_ptr<Response> &parsed_response)
{
  save_pos();
  if (!match("*"))
    PARSE_FAIL
  if (!match(" "))
    PARSE_FAIL
  if (parse_resp_cond_state(parsed_response))
  {
    parsed_response->set_tag("");
  }
  else if (parse_resp_cond_bye(parsed_response))
  {
    parsed_response->set_tag("");
  }
  else if (parse_mailbox_data(parsed_response))
  {
  }
  else if (parse_message_data(parsed_response)){}
  // else if (parse_capability_data()){}
  else
  {
    PARSE_FAIL
  }

  if (!match_crlf())
    PARSE_FAIL

  PARSE_SUCCESS
}

// response = *(continue-req / response-data) response-done
// (parsed line-by-line as) response = continue-req / response-data / response-done
bool ResponseParser::parse_response(std::unique_ptr<Response> &parsed_response)
{
  save_pos();
  if (parse_continue_req())
  {
    PARSE_SUCCESS
  }
  else if (parse_response_data(parsed_response))
  {
    PARSE_SUCCESS
  }
  else if (parse_response_done(parsed_response))
  {
    PARSE_SUCCESS
  }
  else
  {
    PARSE_FAIL
  }

  pop_pos();
  return true;
}


bool ResponseParser::parse_resp_cond_auth(std::unique_ptr<Response> &parsed_response)
{
  save_pos();
  ResponseType type;
  if (match("OK"))
    type = ResponseType::OK;
  else if (match("PREAUTH"))
    type = ResponseType::PREAUTH;
  else
    PARSE_FAIL;
  EXPECT_MATCH(" ");
  if (!parse_resp_text())
    PARSE_FAIL
  parsed_response = std::make_unique<StatusResponse>(type, "", "");
  PARSE_SUCCESS
}

bool ResponseParser::parse_greeting(std::unique_ptr<Response> &parsed_response)
{
  save_pos();
  EXPECT_MATCH("*");
  EXPECT_MATCH(" ");
  if (!parse_resp_cond_auth(parsed_response))
  {
    PARSE_FAIL
  }
  PARSE_SUCCESS
}

bool ResponseParser::parse_next(std::unique_ptr<Response> &parsed_response)
{
  size_t start_pos = curr_pos;
  if (curr_pos >= data.size())
    return false; // Reached end of data

  if(parse_response(parsed_response))
  {
  }
  else if(parse_greeting(parsed_response))
  {
  }
  else
  {
    logger.error_log("Parsing failed.");
    throw std::runtime_error(std::format("Didn't understand the server's response! Response data: {}", data.substr(start_pos, data.size())));
  }
  assert(pos_stack.empty());
  assert(parsed_response != nullptr);
  logger.debug_log("Parsing succeeded.");
  return true;
}
