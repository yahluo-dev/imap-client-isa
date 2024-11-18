/**
 * @file credential.cpp
 * @brief Class representing user's supplied credentials
 * @author Iaroslav Vasilevskii (xvasil10)
 */
#include <fstream>
#include "credential.hpp"

std::regex cred_file_username_regex("username *= *([a-zA-Z0-9@.-]{1,256})", std::regex_constants::ECMAScript);
std::regex cred_file_password_regex("password *= *([a-zA-Z0-9]{1,20})", std::regex_constants::ECMAScript);

std::string Credentials::get_username()
{
  return username;
}

std::string Credentials::get_password()
{
  return password;
}

Credentials::Credentials(std::string _cred_file)
{
  std::ifstream fs(_cred_file);
  std::string line;
  std::smatch match;

  if (!fs)
  {
    throw std::runtime_error("Could not open credential file " + _cred_file);
  }
  std::getline(fs, line);
  if (std::regex_search(line, match, cred_file_username_regex))
  {
    username = match.str(1);
  }
  else
  {
    throw std::runtime_error("Credential file invalid: " + _cred_file);
  }

  std::getline(fs, line);
  if (std::regex_search(line, match, cred_file_password_regex))
  {
    password = match.str(1);
  }
  else
  {
    throw std::runtime_error("Credential file invalid: " +  _cred_file);
  }
  fs.close();
}
