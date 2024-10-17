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
  EXISTS,
  RECENT,
};

class Response
{
  protected:
  ResponseType type;
  public:
  Response(ResponseType _type);
  virtual ResponseType get_type();
  virtual std::string get_tag();
  virtual std::string get_text();
  virtual void set_tag(std::string tag);
  virtual ~Response() = default;
};

class StatusResponse : public Response // OK, NO, BAD, PREAUTH, BYE
{
  private:
  public:
  std::string text;
  std::string tag;
  StatusResponse(ResponseType _type, std::string _tag, std::string text);
  std::string get_text();
  ResponseType get_type();
  std::string get_tag();
  void set_tag(std::string _tag);
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
  std::string message_data;
  public:
  FetchResponse(std::string _message_data);
  std::string get_message_data();
};

class SingleNumberResponse : public Response // EXISTS, RECENT
{
  private:
  uint32_t number;
  public:
  uint32_t get_number();
  SingleNumberResponse(ResponseType _type, uint32_t _number)
    : Response(_type), number(_number){};
};

class FlagsResponse : public Response // FLAGS
{
};

#endif // RESPONSE_H_
