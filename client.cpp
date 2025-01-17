/**
 * @file client.cpp
 * @brief Class implementing a REPL to interact with the user.
 * @author Iaroslav Vasilevskii (xvasil10)
 */
#include <iostream>
#include "client.hpp"
#include "fnv.hpp"
#include "imf_message.hpp"

Client::Client(std::unique_ptr<Session> _session, std::string _mail_dir)
  : session(std::move(_session)), logger(std::cerr), directoryWriter(_mail_dir)
{}

void Client::print_prompt()
{
  ImapState state = session->get_state();
  if (state == ImapState::SELECTED)
  {
    std::cout << "[" << state << " " << session->get_current_mailbox() << "] " << "imapcl> ";
  }
  else
  {
    std::cout << "[" << state << "] " << "imapcl> ";
  }
}

void Client::download_new(const std::smatch &match)
{
  std::string mailbox_name = match[2];
  if (!mailbox_name.empty())
  {
    session->select(mailbox_name);
  }
  std::vector<uint32_t> seq_set = session->search(true);
  if (!seq_set.empty())
  {
    try
    {
      std::vector<IMFMessage> messages = session->fetch(seq_set, false);
      directoryWriter.save_messages(messages);
    }
    catch (std::exception &ex)
    {
      logger.error_log(ex.what());
      return;
    }
  }
  std::cout << seq_set.size() << " new messages "
            << (mailbox_name.empty() ? "" : ("from " + mailbox_name + " "))
            << "downloaded." << std::endl;
}

void Client::download_all(const std::smatch &match)
{
  std::string mailbox_name = match[2];
  if (!mailbox_name.empty())
  {
    session->select(mailbox_name);
  }
  std::vector<uint32_t> seq_set = session->search(false);
  if (!seq_set.empty())
  {
    try
    {
      std::vector<IMFMessage> messages = session->fetch(seq_set, false);
      directoryWriter.save_messages(messages);

    }
    catch (std::exception &ex)
    {
      logger.error_log(ex.what());
      return;
    }
  }
  std::cout << seq_set.size() << " messages "
            << (mailbox_name.empty() ? "" : ("from " + mailbox_name + " "))
            << "downloaded." << std::endl;
}

void Client::read_new(const std::smatch &match)
{
  std::string mailbox_name = match[2];
  if (!mailbox_name.empty())
  {
    session->select(mailbox_name);
  }
  std::vector<uint32_t> seq_set = session->search(false);
  try
  {
    session->read(seq_set);
  }
  catch (std::exception &ex)
  {
    logger.error_log(ex.what());
    return;
  }
  std::cout << "All messages "
            << (mailbox_name.empty() ? "" : ("in " + mailbox_name + " "))
            << "marked as read." << std::endl;
}

void Client::process_command(const std::string &input)
{
  std::smatch match;

  if (std::regex_search(input, match, std::regex("HELP")))
  {
    std::cout << HELP;
  }
  else if (std::regex_search(input, match, std::regex("QUIT")))
  {
    session->logout();
    std::cout << "Logout." << std::endl;
    return;
  }
  else if (std::regex_search(input, match, Commands::DOWNLOADNEW))
  {
    download_new(match);
  }
  else if (std::regex_search(input, match, Commands::DOWNLOADALL))
  {
    download_all(match);
  }
  else if (std::regex_search(input, match, Commands::READNEW))
  {
    read_new(match);
  }
  else if (input.empty())
  {
    return;
  }
  else
  {
    std::cout << "Please use one of the commands: \n" << HELP;
  }
}

void Client::repl()
{
  std::cout << "Interactive mode. Type HELP to get help. Exit with ^D or by typing QUIT." << std::endl;

  std::string input;
  while(true)
  {
    if (session->get_state() == ImapState::SERVER_BYED)
    {
      return;
    }
    print_prompt();
    std::getline(std::cin, input);
    if (std::cin.eof())
    {
      session->logout();
      std::cout << "Closing session: Bye!" << std::endl;
      std::cout << std::endl;
      return;
    }

    process_command(input);
  }
}
