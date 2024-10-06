#include "session.hpp"

Session::Session(const std::string _username, const std::string _password)
  : username(_username), password(_password)
{
  // TODO
}

TLSSession::TLSSession(const std::string _username, const std::string _password)
  : Session(_username, _password)
{
  // TODO
}
