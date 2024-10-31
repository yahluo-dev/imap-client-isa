#include <iostream>
#include <fstream>
#include <unistd.h>
#include <regex>
#include <stdexcept>
#include <fstream>
#include "main.hpp"
#include "server.hpp"
#include "tls_server.hpp"
#include "session.hpp"
#include "fnv.hpp"
#include "tls_receiver.hpp"
#include "credential.hpp"

int main(int argc, char *argv[])
{
  int opt_char;
  std::string port_number;
  std::string server_hostname;
  bool use_tls = false;

  Logger logger(std::cerr);

  bool only_unseen = false;
  bool only_headers = false;
  std::string mailbox_name = "INBOX";
  std::string cert_dir = "/etc/ssl/certs";
  std::string cert_file;
  std::string output_dir;
  std::string auth_file;

  while (-1 != (opt_char = getopt(argc, argv, "p:Tc:C:nha:b:o:")))
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

  if (output_dir.empty())
  {
    logger.error_log("Output directory must be specified.");
    return 1;
  }

  if (optind == argc)
  {
    logger.error_log("Hostname must be supplied.");
    return 1;
  }

  server_hostname = std::string(argv[optind]);
  if (optind != argc-1)
  {
    logger.error_log("Invalid usage: Too many arguments");
    return 1;
  }

  Credentials creds = Credentials(auth_file);

  if (auth_file.empty())
  {
    logger.error_log("Credentials file must be specified.");
  }

  std::unique_ptr<Server> server;
  std::unique_ptr<Receiver> receiver;

  try
  {
    if (use_tls)
    {
      server = std::make_unique<TLSServer>(server_hostname, port_number, cert_file, cert_dir);
      receiver = std::make_unique<TLSReceiver>(dynamic_cast<TLSServer &>(*server));
    }
    else
    {
      server = std::make_unique<Server>(server_hostname, port_number);
      receiver = std::make_unique<Receiver>(*server);
    }
  }
  catch (std::exception &ex)
  {
    logger.error_log(ex.what());
    return 1;
  }

  std::unique_ptr<Session> session = std::make_unique<Session>(std::move(server), std::move(receiver));

  std::vector<uint32_t> seq_set;
  std::vector<std::string> messages;
  try
  {
    session->receive_greeting();
    session->login(creds);
    session->select(mailbox_name);
    seq_set = session->search(only_unseen);
    messages = session->fetch(seq_set, only_headers);
  }
  catch (std::exception &ex)
  {
    logger.error_log(ex.what());
    return 1;
  }

  std::cout << "Fetched " << messages.size() <<
                (only_unseen ? "unseen messages" : "messages") << "." << std::endl;

  FNV fnv;
  for (const auto& message : messages)
  {
    std::string path = output_dir + "/" + fnv.hash(message) + ".eml";
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
