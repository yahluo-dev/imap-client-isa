#include <iostream>
#include "client.hpp"
#include "fnv.hpp"
#include "imf_message.hpp"

const std::regex Client::Commands::DOWNLOADNEW("DOWNLOADNEW( ([a-zA-Z0-9-_]{1,128}))?", std::regex_constants::ECMAScript);
const std::regex Client::Commands::DOWNLOADALL("DOWNLOADALL( ([a-zA-Z0-9-_]{1,128}))?", std::regex_constants::ECMAScript);
const std::regex Client::Commands::READNEW("READNEW( ([a-zA-Z0-9-_]{1,128}))?", std::regex_constants::ECMAScript);

Client::Client(std::unique_ptr<Session> _session, std::string _mail_dir)
  : session(std::move(_session)), mail_dir(_mail_dir), logger(std::cerr)
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

void Client::save_mail(std::vector<std::string> messages)
{
  FNV fnv;
  for (const auto& message : messages)
  {
    IMFMessage imf_message(message);
    std::string path = mail_dir + "/" + fnv.hash(message) +
      imf_message.get_datetime_formatted() + ".eml";
    std::ofstream save_to(path);
    if (!save_to)
    {
      logger.error_log("Could not open " + path + " for writing.");
      return;
    }

    save_to << message << std::endl;
    save_to.close();
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
    std::smatch match;
    if (std::cin.eof())
    {
      session->logout();
      std::cout << "Closing session: Bye!" << std::endl;
      std::cout << std::endl;
      return;
    }
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
          std::vector<std::string> messages = session->fetch(seq_set, false);
          save_mail(messages);
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
    else if (std::regex_search(input, match, Commands::DOWNLOADALL))
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
          std::vector<std::string> messages = session->fetch(seq_set, false);
          save_mail(messages);
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
    else if (std::regex_search(input, match, Commands::READNEW))
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
    else if (input.empty())
    {
      continue;
    }
    else
    {
      std::cout << "Please use one of the commands: \n" << HELP;
    }
  }
}
