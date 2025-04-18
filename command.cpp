/**
 * @file command.cpp
 * @brief Command class representing commands sent by the user
 * @author Iaroslav Vasilevskii (xvasil10)
 */
#include <stdexcept>
#include <sstream>

#include "command.hpp"

std::string Command::make_tcp()
{
  throw std::logic_error("make_tcp() virtual method called!");
}

command_type_t Command::get_type()
{
  return type;
}

Command::Command(std::string _tag) : tag(_tag)
{}

LoginCommand::LoginCommand(std::string _tag, std::string _user_name, std::string _password)
  : Command(_tag), user_name(_user_name), password(_password)
{}

SelectCommand::SelectCommand(std::string _tag, std::string _mailbox_name)
  : Command(_tag), mailbox_name(_mailbox_name)
{}

SearchCommand::SearchCommand(std::string _tag, std::string _search_criteria)
  : Command(_tag), search_criteria(_search_criteria)
{}

FetchCommand::FetchCommand(std::string _tag, std::vector<uint32_t> _sequence_set, std::string _data_item)
  : Command(_tag), sequence_set(_sequence_set), data_item(_data_item)
{}

StoreCommand::StoreCommand(std::string _tag, std::vector<uint32_t> _sequence_set,
             std::string _data_item_name, std::string _data_item_value)
  : Command(_tag), sequence_set(_sequence_set), data_item_name(_data_item_name), data_item_value(_data_item_value)
{};

LogoutCommand::LogoutCommand(std::string _tag)
  : Command(_tag)
{};

std::string LoginCommand::make_tcp()
{
  std::stringstream ss;
  ss << tag << " " << "LOGIN" << " " << user_name << " " << password << "\r\n";

  return ss.str();
}

std::string SelectCommand::make_tcp()
{
  std::stringstream ss;
  ss << tag << " SELECT " << mailbox_name << "\r\n";

  return ss.str();
}

std::string SearchCommand::make_tcp()
{
  std::stringstream ss;
  ss << tag << " SEARCH " << search_criteria << "\r\n";

  return ss.str();
}

std::string FetchCommand::make_tcp()
{
  std::stringstream ss;
  ss << tag << " FETCH";
  if (sequence_set.empty())
    throw std::logic_error("Sequence set empty!");
  ss << " " << sequence_set[0];
  for (auto it = sequence_set.begin() + 1; it != sequence_set.end(); ++it)
  {
    ss << "," << *it;
  }
  ss << " " << data_item << "\r\n";

  return ss.str();
}

std::string StoreCommand::make_tcp()
{
  std::stringstream ss;
  ss << tag << " STORE";
  if (sequence_set.empty())
    throw std::logic_error("Sequence set empty!");
  ss << " " << sequence_set[0];
  for (auto it = sequence_set.begin() + 1; it != sequence_set.end(); ++it)
  {
    ss << "," << *it;
  }
  ss << " " << data_item_name << " " << data_item_value << "\r\n";

  return ss.str();
}

std::string LogoutCommand::make_tcp()
{
  std::stringstream ss;
  ss << tag << " LOGOUT" << "\r\n";
  return ss.str();
}
