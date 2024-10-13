#include <stdexcept>
#include "response.hpp"


Response::Response(ResponseType _type)
: type(_type)
{
}

ResponseType Response::get_type()
{
  throw std::logic_error("get_type virtual method called!");
}

std::string Response::get_tag()
{
  throw std::logic_error("get_tag virtual method called!");
}

StatusResponse::StatusResponse(ResponseType _type, std::string _tag, std::string _text)
  : text(_text), Response(_type)
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

FetchResponse::FetchResponse(std::string _from, std::string _to, std::string _subject, std::string _message_id, std::string _body)
  : Response(ResponseType::FETCH)
{
  message_data = new IMFMessage(_from, _to, _subject, _message_id, _body);
}

std::string FetchResponse::get_message_data()
{
  throw std::logic_error("FetchResponse::get_message_data(): Not implemented");
}
