#include <regex>
#include <stdexcept>
#include "imf_message.hpp"

std::regex date_pattern("^Date:([^\r\n]+)\r\n", std::regex::ECMAScript | std::regex::multiline);

std::string IMFMessage::get_datetime_formatted() const
{
  std::smatch match;
  std::regex_search(data, match, date_pattern);
  if (match.size() == 0)
  {
    throw std::runtime_error("Date header not found!");
  }
  std::string result = match[1];
  return std::regex_replace(result, std::regex("[, :]"), "_");
}
std::string IMFMessage::get_from_formatted() const
{
  throw std::logic_error("Not implemented");
}
std::string IMFMessage::get_data() const
{
  return data;
}
