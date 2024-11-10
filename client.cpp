#include <iostream>
#include "client.hpp"
#include "fnv.hpp"

const std::regex Client::Commands::DOWNLOADNEW("DOWNLOADNEW( ([a-zA-Z0-9-_]{1,128}))?", std::regex_constants::ECMAScript);
const std::regex Client::Commands::DOWNLOADALL("DOWNLOADALL( ([a-zA-Z0-9-_]{1,128}))?", std::regex_constants::ECMAScript);
const std::regex Client::Commands::READNEW("READNEW", std::regex_constants::ECMAScript);

Client::Client(std::unique_ptr<Session> _session, std::string _mail_dir)
  : session(std::move(_session)), mail_dir(_mail_dir), logger(std::cerr)
{}

void Client::print_prompt()
{
  std::cout << "[" << session->get_state() << "] " << "imapcl> ";
}

void Client::save_mail(std::vector<std::string> messages)
{
  FNV fnv;
  for (const auto& message : messages)
  {
    std::string path = mail_dir + "/" + fnv.hash(message) + ".eml";
    std::ofstream save_to(path);
    if (!save_to)
    {
      logger.error_log("Could not open " + path + " for writing.");
      throw std::runtime_error("Could not open " + path + " for writing.");
    }

    save_to << message << std::endl;
    save_to.close();
  }
}

void Client::repl()
{
  std::cout << "Interactive mode. Type HELP to get help. Exit with ^D." << std::endl;

  std::string input;
  while(true)
  {
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
      if (mailbox_name.size() > 0)
      {
        session->select(mailbox_name);
      }
      std::vector<uint32_t> seq_set = session->search(true);
      std::vector<std::string> messages = session->fetch(seq_set, false);
      save_mail(messages);
    }
    else if (std::regex_search(input, match, Commands::DOWNLOADALL))
    {
      std::string mailbox_name = match[2];
      if (mailbox_name.size() > 0)
      {
        session->select(mailbox_name);
      }
      std::vector<uint32_t> seq_set = session->search(false);
      std::vector<std::string> messages = session->fetch(seq_set, false);
      save_mail(messages);
    }
    else if (std::regex_search(input, match, Commands::READNEW))
    {
      std::string mailbox_name = match[2];
      if (mailbox_name.size() > 0)
      {
        session->select(mailbox_name);
      }
      std::vector<uint32_t> seq_set = session->search(false);
      session->read(seq_set);
      std::cout << "All messages marked as read." << std::endl;
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
