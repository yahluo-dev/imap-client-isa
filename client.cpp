#include <iostream>
#include "client.hpp"
#include "fnv.hpp"


Client::Client(std::unique_ptr<Session> _session, std::string _mail_dir)
  : session(std::move(_session)), mail_dir(_mail_dir), logger(std::cerr)
{
}

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
  std::cout << "Type HELP to get help. Exit with ^D." << std::endl;

  std::string input;
  while(true)
  {
    print_prompt();
    std::getline(std::cin, input);
    std::smatch match;
    if (std::cin.eof())
    {
      session->bye();
      std::cout << "Closing session: Bye!" << std::endl;
      std::cout << std::endl;
      return;
    }
    if (std::regex_search(input, match, Commands::DOWNLOADNEW))
    {
      std::vector<uint32_t> seq_set = session->search(true);
      std::vector<std::string> messages = session->fetch(seq_set, false);
    }
    else if (std::regex_search(input, match, Commands::DOWNLOADALL))
    {
      std::vector<uint32_t> seq_set = session->search(false);
      std::vector<std::string> messages = session->fetch(seq_set, false);
    }
    else if (std::regex_search(input, match, Commands::READNEW))
    {
      std::vector<uint32_t> seq_set = session->search(false);
      session->read_new(seq_set); // Not necessary to include the set
      std::cout << "All messages marked as read." << std::endl;
    }
    else if (input.empty())
    {
      continue;
    }
  }
}
