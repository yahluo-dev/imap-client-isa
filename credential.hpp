/**
 * @file credential.hpp
 * @brief Class representing user's supplied credentials
 * @author Iaroslav Vasilevskii (xvasil10)
 */
#ifndef CREDENTIAL_H_
#define CREDENTIAL_H_
#include <regex>
#include <string>


class Credentials
{
  std::string username;
  std::string password;
  public:
  Credentials() = default;
  Credentials(std::string _cred_file);
  std::string get_username();
  std::string get_password();
};

#endif // CREDENTIAL_H_
