#ifndef RESPONSE_PARSER_H_
#define RESPONSE_PARSER_H_

#include <string>
#include <stack>
#include <regex>
#include <memory>

#include "response.hpp"

class ResponseParser
{
  private:
  std::string data;
  int curr_pos;
  std::stack<int> pos_stack;
  void save_pos();
  void restore_pos();
  void pop_pos();
  public:
  ResponseParser(std::string _data)
    : data(_data), curr_pos(0){};

  bool match(std::string expected);
  bool regex_match(std::regex expected);
  bool parse_text();
  bool parse_resp_cond_auth();
  bool parse_greeting();
  bool parse_text_char();
  bool parse_resp_text();
  bool parse_mailbox();
  bool parse_message_id_list();
  bool parse_resp_text_code();
  bool match_crlf();
  bool parse_tag();
  bool parse_response(std::unique_ptr<Response> &parsed_response);
  bool parse_number(int &number);
  bool parse_continue_req();
  bool parse_response_data(std::unique_ptr<Response> &parsed_response);
  bool parse_response_done(std::unique_ptr<Response> &parsed_response);
  bool parse_response_fatal(std::unique_ptr<Response> &parsed_response);
  bool parse_response_tagged(std::unique_ptr<Response> &parsed_response);
  bool parse_response_cond_auth();
  bool parse_resp_cond_state(std::unique_ptr<Response> &parsed_response);
  bool parse_resp_cond_bye(std::unique_ptr<Response> &parsed_response);
  bool parse_mailbox_data(std::unique_ptr<Response> &parsed_response);
  bool parse_flag_list();
  bool parse_mailbox_list();
  std::unique_ptr<Response> parse();

  bool parse_astring();
  bool parse_string();
  bool parse_quoted();
};

#endif // RESPONSE_PARSER_H_
