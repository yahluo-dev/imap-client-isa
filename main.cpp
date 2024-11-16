#include <iostream>
#include <fstream>
#include <regex>
#include <stdexcept>
#include <fstream>
#include <filesystem>
#include <unistd.h>

#include "main.hpp"
#include "server.hpp"
#include "tls_server.hpp"
#include "session.hpp"
#include "fnv.hpp"
#include "tls_receiver.hpp"
#include "credential.hpp"
#include "imf_message.hpp"
#include "client.hpp"

int main(int argc, char *argv[])
{
  int opt_char;
  std::string port_number;
  std::string server_hostname;
  bool use_tls = false;

  Logger logger(std::cerr);

  bool only_unseen = false;
  bool only_headers = false;
  bool interactive = false;
  std::string mailbox_name = "INBOX";
  std::string cert_dir = "/etc/ssl/certs";
  std::string cert_file;
  std::string output_dir;
  std::string auth_file;

  while (-1 != (opt_char = getopt(argc, argv, "p:Tc:C:nha:b:o:i")))
  {
    switch(opt_char)
    {
      case 'p':
      {
        long arg_port = strtol(optarg, nullptr, 10);
        if (arg_port > 65535 || arg_port < 1)
        {
          logger.error_log("-s: PORT must be in range <1, 65535>!");
          exit(1);
        }
        port_number = optarg;
        break;
      }
      case 'T':
      {
        use_tls = true;
        break;
      }
      case 'a':
      {
        auth_file = optarg;
        break;
      }
      case 'i':
      {
        interactive = true;
        break;
      }
      case 'n':
      {
        only_unseen = true;
        break;
      }
      case 'h':
      {
        only_headers = true;
        break;
      }
      case 'b':
      {
        mailbox_name = optarg;
        break;
      }
      case 'o':
      {
        output_dir = optarg;
        break;
      }
      case 'C':
      {
        cert_dir = optarg;
        break;
      }
      case 'c':
      {
        cert_file = optarg;
        break;
      }
      default:
      {
        logger.error_log("Invalid usage.");
        std::cout << USAGE << std::endl;
        return 1;
        break;
      }
    }
  }

  if (optind == argc)
  {
    logger.error_log("Hostname must be supplied.");
    std::cout << USAGE << std::endl;
    return 1;
  }

  if (output_dir.empty())
  {
    logger.error_log("Output directory must be specified.");
    std::cout << USAGE << std::endl;
    return 1;
  }
  else if (!std::filesystem::exists(output_dir))
  {
    logger.error_log("Output directory does not exist.");
    return 1;
  }
  else if (!std::filesystem::is_directory(output_dir))
  {
    logger.error_log("Output directory is not a directory!");
    return 1;
  }

  server_hostname = std::string(argv[optind]);
  if (optind != argc-1)
  {
    logger.error_log("Invalid usage: Too many arguments");
    std::cout << USAGE << std::endl;
    return 1;
  }

  // Use default ports
  if (port_number.empty())
  {
    if (use_tls)
    {
      port_number = "993";
    }
    else
    {
      port_number = "143";
    }
  }

  if (auth_file.empty())
  {
    logger.error_log("Credentials file must be specified.");
    std::cout << USAGE << std::endl;
    return 1;
  }

  Credentials creds;

  try
  {
    creds = Credentials(auth_file);
  }
  catch (std::runtime_error &ex)
  {
    logger.error_log(ex.what());
    return 1;
  }

  std::unique_ptr<Server> server;

  try
  {
    if (use_tls)
    {
      server = std::make_unique<TLSServer>(server_hostname, port_number, cert_file, cert_dir);
    }
    else
    {
      server = std::make_unique<Server>(server_hostname, port_number);
    }
  }
  catch (std::exception &ex)
  {
    logger.error_log(ex.what());
    return 1;
  }

  std::unique_ptr<Session> session = std::make_unique<Session>();
  session->connect(std::move(server));

  if (interactive)
  {
    session->receive_greeting();
    session->login(creds);
    session->select(mailbox_name);
    std::unique_ptr<Client> client = std::make_unique<Client>(std::move(session), output_dir);
    try
    {
      client->repl();
    }
    catch (std::exception &ex)
    {
      logger.error_log(ex.what());
      return 1;
    }
  }
  else
  {
    std::vector<uint32_t> seq_set;
    std::vector<std::string> messages;
    try
    {
      session->receive_greeting();
      session->login(creds);
      session->select(mailbox_name);
      seq_set = session->search(only_unseen);
      if (seq_set.size() > 0)
      {
        messages = session->fetch(seq_set, only_headers);
      }
    }
    catch (std::exception &ex)
    {
      logger.error_log(ex.what());
      return 1;
    }

    size_t n_messages = messages.size();
    if (0 != n_messages)
    {
      std::cout << "* Fetched " << n_messages << " " <<
                    (only_unseen ? "unseen messages" : "messages") <<
                    (only_headers ? " (headers)" : "") << "." << std::endl;
    }
    else
    {
      std::cout << "* " << "No" << (only_unseen ? " unseen" : "") << " messages " <<
        "in mailbox \"" << mailbox_name << "\"" << std::endl;
      return 0;
    }

    FNV fnv;
    for (const auto& message : messages)
    {
      IMFMessage imf_message(message);
      std::string path = output_dir + "/" + fnv.hash(message) +
        imf_message.get_datetime_formatted() + ".eml";
      std::ofstream save_to(path);
      if (!save_to)
      {
        logger.error_log("Could not open " + path + " for writing.");
        return 1;
      }

      save_to << message << std::endl;
      save_to.close();
    }
  
  }


  return 0;
}
