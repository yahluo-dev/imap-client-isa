#include <stdexcept>
#include "response.hpp"


Response::Response(std::string _tag)
  : tag(_tag)
{
}

response_type_t Response::get_type()
{
  throw std::logic_error("get_type virtual method called!");
}

std::string Response::get_tag()
{
  throw std::logic_error("get_tag virtual method called!");
}

TextResponse::TextResponse(std::string _text)
  : text(_text)
{}

std::string TextResponse::get_text()
{
  return text;
}

SearchResponse::SearchResponse(std::vector<uint32_t> _seq_numbers)
  : seq_numbers(_seq_numbers)
{}

std::vector<uint32_t> SearchResponse::get_seq_numbers()
{
  return seq_numbers;
}

FetchResponse::FetchResponse(std::string _from, std::string _to, std::string _subject, std::string _message_id, std::string _body)
{
  message_data = new IMFMessage(_from, _to, _subject, _message_id, _body);
}

std::string FetchResponse::get_message_data()
{
  throw std::logic_error("FetchResponse::get_message_data(): Not implemented");
}
