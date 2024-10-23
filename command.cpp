#include <stdexcept>
#include <format>

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

std::string LoginCommand::make_tcp()
{
  std::string command_string = std::format("{} LOGIN {} {}\r\n", tag, user_name, password);

  return command_string;
}

std::string SelectCommand::make_tcp()
{
  std::string command_string = std::format("{} SELECT {}\r\n", tag, mailbox_name);

  return command_string;
}

std::string SearchCommand::make_tcp()
{
  std::string command_string = std::format("{} SEARCH {}\r\n", tag, search_criteria);

  return command_string;
}

std::string FetchCommand::make_tcp()
{
  std::string command_string = std::format("{} FETCH", tag);
  command_string += std::format(" {}", sequence_set[0]);
  for (auto it = sequence_set.begin() + 1; it != sequence_set.end(); ++it)
  {
    command_string += std::format(",{}", *it);
  }
  command_string += " ";
  command_string += data_item;
  command_string += "\r\n";

  return command_string;
}
