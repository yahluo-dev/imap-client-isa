#include <iostream>
#include <fstream>
#include <unistd.h>
#include <regex>
#include <stdexcept>
#include "main.hpp"
#include "session.hpp"

typedef struct {
  std::string username;
  std::string password;
} Credential_t;

std::regex cred_file_username_regex("username=([a-zA-Z0-9@.-]{1,256})", std::regex_constants::ECMAScript);
std::regex cred_file_password_regex("password=([a-zA-Z0-9]{1,20})", std::regex_constants::ECMAScript);

Credential_t parse_cred_file(std::string filename)
{
  std::string line;
  std::smatch match;
  std::ifstream auth_file_stream(filename);

  Credential_t credentials;

  std::getline(auth_file_stream, line);

  if (std::regex_search(line, match, cred_file_username_regex))
  {
    credentials.username = match.str(1);
  }
  else
  {
    throw std::runtime_error("Auth file invalid!");
  }

  std::getline(auth_file_stream, line);

  if (std::regex_search(line, match, cred_file_password_regex))
  {
    credentials.password = match.str(1);
  }
  else
  {
    throw std::runtime_error("Auth file invalid!");
  }

  return credentials;
}

int main(int argc, char *argv[])
{
  int opt_char;
  std::string port_number;
  std::string server_hostname;
  bool use_tls = false;

  bool only_new = false;
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
          std::cerr << "-s: PORT must be in range <1, 65535>!" << std::endl;
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
        only_new = true;
        break;
      }
      case 't':
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
      case 'h':
      {
        std::cerr << USAGE << std::endl;
        return 0;
        break;
      }
      default:
      {
        std::cerr << "Invalid Usage or not implemented." << std::endl;
        std::cerr << USAGE << std::endl;
        return 1;
        break;
      }
    }
  }

  if (optind == argc)
  {
    std::cerr << "Hostname must be supplied." << std::endl;
    return 1;
  }

  server_hostname = std::string(argv[optind]);
  if (optind != argc-1)
  {
    std::cout << "Invalid usage: Too many arguments" << std::endl;
  }

  std::cout << "port number: " << port_number << std::endl;
  std::cout << "hostname: " << server_hostname << std::endl;
  std::cout << "use_tls: " << use_tls << std::endl;

  Credential_t creds = parse_cred_file(auth_file);

  if (auth_file.empty())
  {
    std::cout << "Credentials file must be specified." << std::endl;
  }
  else
  {
    std::cout << "Creds: " << creds.username << ":" << creds.password << std::endl;
  }

  std::unique_ptr<Session> session = std::make_unique<Session>(server_hostname, port_number);

}
