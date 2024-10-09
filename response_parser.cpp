#include <cctype>
#include "response_parser.hpp"

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

  if (data[curr_pos] == '\r' || data[curr_pos == '\n'])
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
