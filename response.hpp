#ifndef RESPONSE_H_
#define RESPONSE_H_

#include <string>
#include <vector>
#include <stdint.h>
#include "message.hpp"

enum class ResponseType
{
  OK,
  NO,
  BAD,
  PREAUTH,
  BYE,
  SEARCH,
  FETCH,
};

class Response
{
  protected:
  ResponseType type;
  public:
  Response(ResponseType _type);
  virtual ResponseType get_type();
  virtual std::string get_tag();
  virtual ~Response() = default;
};

class StatusResponse : public Response // OkResponse, NoResponse, BadResponse, PreauthResponse, ByeResponse
{
  private:
  public:
  std::string text;
  std::string tag;
  StatusResponse(ResponseType _type, std::string _tag, std::string text);
  std::string get_text();
  ResponseType get_type();
  std::string get_tag();
};

class SearchResponse : public Response
{
  private:
  std::vector<uint32_t> seq_numbers;
  public:
  SearchResponse(std::vector<uint32_t> _seq_numbers)
    : Response(ResponseType::SEARCH), seq_numbers(_seq_numbers)
  {};
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
