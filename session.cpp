#include <iostream>
#include <sstream>
#include <memory>
#include <stdexcept>

#include "command.hpp"
#include "response.hpp"
#include "session.hpp"
#include "server.hpp"

std::mutex Session::incoming_mutex;
std::condition_variable Session::incoming_cv;
std::queue<std::unique_ptr<Response>> Session::response_queue;

std::ostream& operator<<(std::ostream& os, ImapState response_type)
{
  switch(response_type)
  {
    case ImapState::GREETING:
      os << std::string("GREETING");
      break;
    case ImapState::NAUTH:
      os << std::string("NAUTH");
      break;
    case ImapState::AUTHD:
      os << std::string("AUTHD");
      break;
    case ImapState::SELECTED:
      os << std::string("SELECTED");
      break;
    case ImapState::LOGOUT:
      os << std::string("LOGOUT");
      break;
    case ImapState::RECEIVER_ERROR:
      os << std::string("RECEIVER_ERROR");
      break;
    case ImapState::SERVER_BYED:
      os << std::string("SERVER_BYED");
      break;
  }
  return os;
}

void Session::notify_incoming(std::unique_ptr<Response> response)
{
  std::lock_guard<std::mutex> lg(incoming_mutex);
  ResponseType response_type = response->get_type();
  switch(response_type)
  {
    case ResponseType::BYE:
      if (state != ImapState::LOGOUT)
      {
        logger.error_log("[Server BYE] " + response->get_text());
        transition(ImapState::SERVER_BYED);
      }
      else
      {
        response_queue.push(std::move(response));
      }
      break;
    default:
      response_queue.push(std::move(response));
      break;
  }
  incoming_cv.notify_one();
}

void Session::receiver_notify_failed(std::exception &ex)
{
  receiver_ex = ex;
  transition(ImapState::RECEIVER_ERROR);
  incoming_cv.notify_one();
}

std::unique_ptr<Response> Session::wait_for_response()
{
  std::unique_lock ul(incoming_mutex);

  if (!incoming_cv.wait_for(ul, std::chrono::seconds(3), [this] { return (!response_queue.empty() ||
                                                                       this->state == ImapState::RECEIVER_ERROR ||
                                                                       this->state == ImapState::SERVER_BYED); }))
  {
    throw std::runtime_error("Connection established but timed out waiting for response.");
  }
  if (state == ImapState::RECEIVER_ERROR)
  {
    throw std::runtime_error(std::string("Connection error: ") + receiver_ex.what());
  }
  else if (state == ImapState::SERVER_BYED)
  {
    throw std::runtime_error(std::string("Connection terminated by server."));
  }

  std::unique_ptr<Response> response = std::move(response_queue.front());
  response_queue.pop();
  return response;
}

std::string Session::get_new_tag()
{
  return std::to_string(tag++);
}

Session::Session() : state(ImapState::GREETING), tag(1), logger(std::cerr)
{}
ImapState Session::get_state()
{
  return state;
}

void Session::connect(std::unique_ptr<Server> _server)
{
  server = std::move(_server);
  server->receive(*this);
}

Session::~Session()
{}

void Session::read(std::vector<uint32_t> sequence_set)
{
  if (state != ImapState::SELECTED)
  {
    throw std::logic_error("Can only issue this command in SELECTED state.");
  }

  server->send(std::make_unique<StoreCommand>(get_new_tag(), sequence_set,
                                              "+FLAGS", "\\Seen"));

  std::unique_ptr<Response> response = wait_for_response();
}

void Session::logout()
{
  server->send(std::make_unique<LogoutCommand>(get_new_tag()));
  transition(ImapState::LOGOUT);
  std::unique_ptr<Response> response = wait_for_response();
}

void Session::transition(ImapState _state)
{
  state = _state;
}

// LOGIN
//    Arguments:  user name
//                password
//    Responses:  no specific responses for this command
//    Result:     OK - login completed, now in authenticated state
//                NO - login failure: user name or password rejected
//                BAD - command unknown or arguments invalid
void Session::login(Credentials &creds)
{
  if (state == ImapState::AUTHD)
  {
    return;
  }
  else if (state != ImapState::NAUTH)
  {
    throw std::logic_error("Cannot issue this command in current state.");
  }

  std::unique_ptr<LoginCommand> login_command = std::make_unique<LoginCommand>("a001", creds.get_username(), creds.get_password()); // FIXME

  server->send(std::move(login_command));

  std::unique_ptr<Response> second_response = wait_for_response();

  if (second_response->get_type() == ResponseType::OK)
  {
    if (second_response->get_tag() != "a001")
    {
      throw std::runtime_error("Unexpected tag");
    }
    else
    {
      logger.debug_log("[OK] Server: " + second_response->get_text());
      transition(ImapState::AUTHD);
    }
  }
  else
  {
      logger.error_log("[" + responseTypeToString(second_response->get_type()) + "] Server: " + second_response->get_text());
      throw std::runtime_error("LOGIN failed.");
  }
}

void Session::unexpected_response(std::unique_ptr<Response> &response)
{
  std::stringstream ss;
  ss << "Unexpected response in state " << state << ": " <<response->get_type();
  throw std::runtime_error(ss.str());
}

void Session::receive_greeting()
{
  std::unique_ptr<Response> greeting = wait_for_response();

  switch(greeting->get_type())
  {
    case ResponseType::OK:
      transition(ImapState::NAUTH);
      break;
    case ResponseType::PREAUTH:
      transition(ImapState::AUTHD); // Preauthenticated connection
      break;
    default:
      unexpected_response(greeting);
      break;
  }
}


// SELECT
//    Arguments:  mailbox name
//    Responses:  REQUIRED untagged responses: FLAGS, EXISTS, RECENT
//                REQUIRED OK untagged responses:  UNSEEN,  PERMANENTFLAGS,
//                UIDNEXT, UIDVALIDITY
//    Result:     OK - select completed, now in selected state
//                NO - select failure, now in authenticated state: no
//                     such mailbox, can't access mailbox
//                BAD - command unknown or arguments invalid
void Session::select(const std::string mailbox)
{
  if (state == ImapState::NAUTH)
  {
    throw std::logic_error("Cannot issue this command in current state.");
  }
  server->send(std::make_unique<SelectCommand>(get_new_tag(), mailbox));

  std::unique_ptr<Response> response;
  do // Skip through untagged responses
  {
    response = wait_for_response(); // FIXME IMPORTANT: Account for unsolicited data. Do we need concurrence because of that though? Unsolicited data is *always untagged*
  } while (response->get_tag() == "");

  // The last one must be the one indicating OK status
  if (response->get_type() == ResponseType::OK)
  {
    transition(ImapState::SELECTED);
    current_mailbox = mailbox;
  }
  else
  {
    logger.error_log("[" + responseTypeToString(response->get_type()) + "] Server: " + response->get_text());
    throw std::runtime_error("SELECT failed.");
  }
}

std::string Session::get_current_mailbox()
{
  return current_mailbox;
}

// SEARCH
//    Arguments:  OPTIONAL [CHARSET] specification
//                searching criteria (one or more)
//    Responses:  REQUIRED untagged response: SEARCH
//    Result:     OK - search completed
//                NO - search error: can't search that [CHARSET] or
//                     criteria
//                BAD - command unknown or arguments invalid
std::vector<uint32_t> Session::search(bool only_unseen)
{
  if (state != ImapState::SELECTED)
  {
    throw std::logic_error("Cannot issue this command in current state.");
  }

  server->send(std::make_unique<SearchCommand>(get_new_tag(),
                                               only_unseen ? "UNSEEN" : "ALL"));

  std::unique_ptr<SearchResponse> search_results_response;
  std::unique_ptr<Response> response;
  do // Skip through untagged responses
  {
    response = wait_for_response(); // FIXME IMPORTANT: Account for unsolicited data. Do we need concurrence because of that though? Unsolicited data is *always untagged*
    if (response->get_type() == ResponseType::SEARCH)
    {
      search_results_response = std::unique_ptr<SearchResponse>(dynamic_cast<SearchResponse *>(response.release()));
      response = std::make_unique<Response>(ResponseType::OK);
    }
  } while (response->get_tag() == "");

  // Extract sequence set. We may have as well failed.
  if (response->get_type() == ResponseType::OK)
  {
    logger.debug_log("Search OK");
    return search_results_response->get_seq_numbers();
  }
  else
  {
    logger.error_log("[" + responseTypeToString(response->get_type()) + "] Server: " + response->get_text());
    throw std::runtime_error("SEARCH failed");
  }
}

// FETCH Command
//    Arguments:  sequence set
//                message data item names or macro
//    Responses:  untagged responses: FETCH
//    Result:     OK - fetch completed
//                NO - fetch error: can't fetch that data
//                BAD - command unknown or arguments invalid
std::vector<std::string> Session::fetch(std::vector<uint32_t> sequence_set, bool only_headers)
{
  if (state != ImapState::SELECTED)
  {
    throw std::logic_error("Cannot issue this command in current state.");
  }

  server->send(std::make_unique<FetchCommand>(get_new_tag(), sequence_set,
                                              only_headers ? "(BODY.PEEK[HEADER])"
                                              : "BODY[]"));

  std::vector<std::unique_ptr<FetchResponse>> fetch_results_responses;
  std::unique_ptr<Response> response;

  do
  {
    response = wait_for_response();
    if (response->get_type() == ResponseType::FETCH)
    {
      fetch_results_responses.push_back(std::unique_ptr<FetchResponse>(dynamic_cast<FetchResponse *>(response.release())));
      response = std::make_unique<Response>(ResponseType::OK);
    }
  } while (response->get_tag() == "");

  if (response->get_type() == ResponseType::OK)
  {
    logger.debug_log("Fetch OK");
    std::vector<std::string> result_vector;
    for (const auto& response : fetch_results_responses)
    {
      std::string message_data = response->get_message_data();
      if (message_data.empty())
      {
        logger.error_log("Server did not send body for message ID " + response->get_tag());
      }
      result_vector.push_back(message_data);
    }
    return result_vector;
  }
  else
  {
    logger.error_log("[" + responseTypeToString(response->get_type()) + "] Server: " + response->get_text());
    throw std::runtime_error("FETCH failed");
  }
}
