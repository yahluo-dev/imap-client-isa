#include <cctype>
#include <iostream>
#include <format>
#include <stdio.h>
#include "response_parser.hpp"

#define PARSE_FAIL {restore_pos();return false;}
#define PARSE_SUCCESS {pop_pos();return false;} // TODO: Replace macros with a decorator

#define EXPECT_MATCH(s) {if (!match(s)){restore_pos();return false;}}

#define debug(...) printf("DEBUG :: " __VA_ARGS__);


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

bool ResponseParser::parse_crlf()
{
  save_pos();
  if (!match("\r"))
  {
    restore_pos();
    return false;
  }

  if (!match("\n"))
  {
    restore_pos();
    return false;
  }
  pop_pos();
  return true;
}

bool ResponseParser::parse_number()
{
  save_pos();
  bool success = false;
  while (std::isdigit(data[curr_pos]))
  {
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
  while (std::isalnum(data[curr_pos])) // FIXME: This is not in line with grammar
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

  std::cout << "Parse text char " << data[curr_pos] << std::endl;
  if (data[curr_pos] == '\r' || data[curr_pos] == '\n')
  {
    debug("from ");
    std::cout << curr_pos << std::endl;
    restore_pos();
    debug("restored pos to ");
    std::cout << curr_pos << std::endl;
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
  debug("parse_text: parsing from ");
  std::cout <<  data.substr(curr_pos, 2) << std::endl;

  if (parse_text_char())
    success = true;

  while(parse_text_char());

  if (success)
  {
    pop_pos();
    debug("parse_text success");
    return true;
  }
  else
  {
    debug("parse_text fail at");
    std::cout <<  data.substr(curr_pos, 2) << std::endl;
    restore_pos();
    return false;
  }
}

// resp-text       = ["[" resp-text-code "]" SP] text
bool ResponseParser::parse_resp_text()
{
  save_pos();
  // TODO: parse_resp_text_code()
  if (!parse_text())
  {
    restore_pos();
    return false;
  }
  pop_pos();
  return true;
}

// resp-cond-bye   = "BYE" SP resp-text
bool ResponseParser::parse_resp_cond_bye()
{
  save_pos();
  if(!match("BYE"))
  {
    restore_pos();
    return false;
  }
  if (!parse_resp_text())
  {
    restore_pos();
    return false;
  }
  pop_pos();
  return true;
}

// resp-cond-state = ("OK" / "NO" / "BAD") SP resp-text
//                     ; Status condition
bool ResponseParser::parse_resp_cond_state()
{
  save_pos();
  if (match("OK"))
  {}
  else if (match("NO"))
  {}
  else if (match("BAD"))
  {}
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

  if (!parse_resp_text())
  {
    restore_pos();
    return false;
  }

  pop_pos();
  return true;
}

// response-tagged = tag SP resp-cond-state CRLF
bool ResponseParser::parse_response_tagged()
{
  save_pos();
  if (!parse_tag())
  {
    restore_pos();
    return false;
  }
  if (!match(" "))
  {
    restore_pos();
    return false;
  }
  if (!parse_resp_cond_state())
  {
    restore_pos();
    return false;
  }
  pop_pos();
  return true;
}

// response-fatal  = "*" SP resp-cond-bye CRLF
//                     ; Server closes connection immediately
bool ResponseParser::parse_response_fatal()
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
  if (!parse_resp_cond_bye())
  {
    restore_pos();
    return false;
  }

  pop_pos();
  return true;
}

// response-done   = response-tagged / response-fatal
bool ResponseParser::parse_response_done()
{
  save_pos();
  if (parse_response_tagged())
  {
    pop_pos();
    return true;
  }
  else if (parse_response_fatal())
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
  if (!parse_crlf())
  {
    restore_pos();
    return false;
  }

  pop_pos();
  return true;
}

bool ResponseParser::parse_flag_list()
{
 // TODO
 return false;
}

bool ResponseParser::parse_mailbox_list()
{
 // TODO
 return false;
}


// mailbox-data    =  "FLAGS" SP flag-list / "LIST" SP mailbox-list /
//                    "LSUB" SP mailbox-list / "SEARCH" *(SP nz-number) /
//                    "STATUS" SP mailbox SP "(" [status-att-list] ")" /
//                    number SP "EXISTS" / number SP "RECENT"
bool ResponseParser::parse_mailbox_data()
{
  save_pos();
  if (match("FLAGS"))
  {
    EXPECT_MATCH(" ");
    if (!parse_flag_list())
    {
      restore_pos();
      return false; // Can this even happen for a valid string?
    }
    pop_pos();
    return true;
  }
  else if (match("LIST"))
  {
    EXPECT_MATCH(" ");
    if (!parse_mailbox_list())
    {
      restore_pos();
      return false;
    }
  }
  else if (match("LSUB"))
  {
    EXPECT_MATCH(" ");
    if (!parse_mailbox_list())
    {
      restore_pos();
      return false;
    }
  }
  else if (match("LSUB"))
  {
    EXPECT_MATCH(" ");
    if (!parse_mailbox_list())
    {
      restore_pos();
      return false;
    }
  }
}


// response-data   = "*" SP (resp-cond-state / resp-cond-bye /
//                   mailbox-data / message-data / capability-data) CRLF
bool ResponseParser::parse_response_data()
{
  save_pos();
  if (!match("*"))
    PARSE_FAIL
  if (!match(" "))
    PARSE_FAIL
  if (parse_resp_cond_state()){}
  else if (parse_resp_cond_bye()){}
  else if (parse_mailbox_data()){}
  // else if (parse_capability_data()){}
  else
  {
    PARSE_FAIL
  }

  if (!parse_crlf())
    PARSE_FAIL

  PARSE_SUCCESS
}

// response = *(continue-req / response-data) response-done
bool ResponseParser::parse_response()
{
  save_pos();
  while (true)
  {
    if (parse_continue_req())
      continue;
    else if (parse_response_data())
      continue;
    else
      break;
  }
  if (!parse_response_done())
  {
    restore_pos();
    return false;
  }

  pop_pos();
  return true;
}

bool ResponseParser::parse()
{
  if(!parse_response())
  {
    return false;
  }
  return true;
}
