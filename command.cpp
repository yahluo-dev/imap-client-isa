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

SearchCommand::SearchCommand(std::string _tag, std::string _search_criteria)
  : Command(_tag), search_criteria(_search_criteria)
{}

FetchCommand::FetchCommand(std::string _tag, std::vector<uint32_t> _sequence_set)
  : Command(_tag), sequence_set(_sequence_set)
{}

std::string LoginCommand::make_tcp()
{
  std::string command_string = std::format("{} LOGIN {} {}", tag, user_name, password);

  return command_string;
}

std::string SelectCommand::make_tcp()
{
  std::string command_string = std::format("{} SELECT {}", tag, mailbox_name);

  return command_string;
}

std::string SearchCommand::make_tcp()
{
  std::string command_string = std::format("{} SEARCH {}", tag, search_criteria);

  return command_string;
}

std::string FetchCommand::make_tcp()
{
  std::string command_string = std::format("{} FETCH", tag);
  for (const auto& num : sequence_set)
  {
    command_string += std::format(" {}", num);
  }

  return command_string;
}
