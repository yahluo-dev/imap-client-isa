#ifndef RESPONSE_H_
#define RESPONSE_H_

#include <string>
#include <vector>
#include <stdint.h>
#include "message.hpp"

enum response_type_t
{
  RESPONSE_OK,
  RESPONSE_NO,
  RESPONSE_BAD,
  RESPONSE_PREAUTH,
  RESPONSE_BYE,
  RESPONSE_SEARCH,
  RESPONSE_FETCH,
};

class Response
{
  protected:
  response_type_t type;
  std::string tag;
  public:
  Response(std::string _tag);
  virtual response_type_t get_type();
  virtual std::string get_tag();
  virtual ~Response() = default;
};

class TextResponse : public Response // OkResponse, NoResponse, BadResponse, PreauthResponse, ByeResponse
{
  private:
  std::string text;
  public:
  TextResponse(std::string text);
  std::string get_text();
};

class SearchResponse : public Response
{
  private:
  std::vector<uint32_t> seq_numbers;
  public:
  SearchResponse(std::vector<uint32_t> _seq_numbers);
  std::vector<uint32_t> get_seq_numbers();
};

class FetchResponse : public Response
{
  private:
  IMFMessage *message_data; // FIXME: What if we only want to fetch headers?
  public:
  FetchResponse(std::string from, std::string to, std::string subject, std::string message_id, std::string body);
  std::string get_message_data();
};

#endif // RESPONSE_H_
