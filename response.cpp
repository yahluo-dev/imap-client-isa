#include <stdexcept>
#include "response.hpp"

std::ostream& operator<<(std::ostream& os, ResponseType response_type)
{
  switch(response_type)
  {
    case ResponseType::OK:
      os << std::string("OK");
      break;
    case ResponseType::NO:
      os << std::string("NO");
      break;
    case ResponseType::BAD:
      os << std::string("BAD");
      break;
    case ResponseType::PREAUTH:
      os << std::string("PREAUTH");
      break;
    case ResponseType::BYE:
      os << std::string("BYE");
      break;
    case ResponseType::SEARCH:
      os << std::string("SEARCH");
      break;
    case ResponseType::FETCH:
      os << std::string("FETCH");
      break;
    case ResponseType::EXISTS:
      os << std::string("EXISTS");
      break;
    case ResponseType::RECENT:
      os << std::string("RECENT");
      break;
    case ResponseType::FLAGS:
      os << std::string("FLAGS");
      break;
  }
  return os;
}

std::string responseTypeToString(ResponseType response_type)
{
  switch(response_type)
  {
    case ResponseType::OK:
      return std::string("OK");
    case ResponseType::NO:
      return std::string("NO");
    case ResponseType::BAD:
      return std::string("BAD");
    case ResponseType::PREAUTH:
      return std::string("PREAUTH");
    case ResponseType::BYE:
      return std::string("BYE");
    case ResponseType::SEARCH:
      return std::string("SEARCH");
    case ResponseType::FETCH:
      return std::string("FETCH");
    case ResponseType::EXISTS:
      return std::string("EXISTS");
    case ResponseType::RECENT:
      return std::string("RECENT");
    case ResponseType::FLAGS:
      return std::string("FLAGS");
  }
}

Response::Response(ResponseType _type)
: type(_type)
{
}

uint32_t SingleNumberResponse::get_number()
{
  return number;
}

void Response::set_tag(std::string _tag)
{
  throw std::logic_error("set_tag virtual method called!");
}

void StatusResponse::set_tag(std::string _tag)
{
  tag = _tag;
}

ResponseType Response::get_type()
{
  return type;
}

std::string Response::get_tag()
{
  return "";
}

std::string Response::get_text()
{
  throw std::logic_error("get_text virtual method called!");
}

StatusResponse::StatusResponse(ResponseType _type, std::string _tag, std::string _text)
  :  Response(_type), text(_text), tag(_tag)
{}

ResponseType StatusResponse::get_type()
{
  return type;
}

std::string StatusResponse::get_tag()
{
  return tag;
}


std::string StatusResponse::get_text()
{
  return text;
}

std::vector<uint32_t> SearchResponse::get_seq_numbers()
{
  return seq_numbers;
}

FetchResponse::FetchResponse(std::string _message_data)
  : Response(ResponseType::FETCH), message_data(_message_data)
{}

std::string FetchResponse::get_message_data()
{
  return message_data;
}
