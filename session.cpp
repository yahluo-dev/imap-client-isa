#include <iostream>
#include <memory>
#include <stdexcept>

#include "command.hpp"
#include "response.hpp"
#include "session.hpp"
#include "server.hpp"

std::string Session::get_new_tag()
{
  return std::to_string(tag++);
}

Session::Session(const std::string hostname, const std::string port)
  : tag(1)
{
  server = std::make_unique<Server>(hostname, port);
}

void Session::login(const std::string username, const std::string password)
{

  std::unique_ptr<Response> greeting = server->receive();

  std::unique_ptr<LoginCommand> login_command = std::make_unique<LoginCommand>("a001", username, password); // FIXME

  server->send(std::move(login_command));

  std::unique_ptr<Response> second_response = server->receive();

  if (second_response->get_type() == ResponseType::OK)
  {
    if (second_response->get_tag() != "a001")
    {
      throw std::runtime_error("Unexpected tag");
    }
    else
    {
      std::cout << "[OK] Server: " << second_response->get_text() << std::endl;
    }
  }
  else
  {
      std::cout << "[NOT OK] Server: " << second_response->get_text() << std::endl;
  }
}


// 6.3.1.  SELECT Command
//
//    Arguments:  mailbox name
//
//    Responses:  REQUIRED untagged responses: FLAGS, EXISTS, RECENT
//                REQUIRED OK untagged responses:  UNSEEN,  PERMANENTFLAGS,
//                UIDNEXT, UIDVALIDITY
//
//    Result:     OK - select completed, now in selected state
//                NO - select failure, now in authenticated state: no
//                     such mailbox, can't access mailbox
//                BAD - command unknown or arguments invalid
void Session::select(const std::string mailbox)
{
  server->send(std::make_unique<SelectCommand>(get_new_tag(), mailbox));

  std::unique_ptr<Response> response;
  do // Skip through untagged responses
  {
    response = server->receive(); // FIXME IMPORTANT: Account for unsolicited data. Do we need concurrence because of that though? Unsolicited data is *always untagged*
  } while (response->get_tag() == "");

  // The last one must be the one indicating OK status
  if (response->get_type() == ResponseType::OK)
  {
  }
  else
  {
    std::cout << "[" << response->get_type() << "] Server: " << response->get_type() << std::endl;
  }
}

std::vector<uint32_t> Session::search()
{
  server->send(std::make_unique<SearchCommand>(get_new_tag(), "ALL"));

  std::unique_ptr<Response> response;
  std::unique_ptr<SearchResponse> search_results_response;
  do // Skip through untagged responses
  {
    response = server->receive(); // FIXME IMPORTANT: Account for unsolicited data. Do we need concurrence because of that though? Unsolicited data is *always untagged*
    if (response->get_type() == ResponseType::SEARCH)
    {
      search_results_response = std::unique_ptr<SearchResponse>(dynamic_cast<SearchResponse *>(response.release()));
      // TODO: What if the server doesn't send it?
    }
  } while (response->get_tag() == "");

  // Extract sequence set. We may have failed too.
  if (response->get_type() == ResponseType::OK)
  {
    std::cout << "Search OK" << std::endl;
    return search_results_response->get_seq_numbers();
  }
  else
  {
    std::cout << "[" << response->get_type() << "] Server: " << response->get_type() << std::endl;
    throw std::runtime_error("Search failed");
  }
}

void Session::fetch(std::vector<uint32_t> sequence_set)
{

}
