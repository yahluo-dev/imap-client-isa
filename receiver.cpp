#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include "receiver.hpp"
#include "response_parser.hpp"


std::string Receiver::received_data;
volatile bool stopped;
int Receiver::client_socket;

Receiver::Receiver()
{
}

Receiver::Receiver(Server &server)
{
  client_socket = server.get_socket();
}

std::string Receiver::receive_inner()
{
  ssize_t bytes_recvd;
  char buffer[RECVMESSAGE_MAXLEN];
  if (-1 == (bytes_recvd = recv(client_socket, &buffer, RECVMESSAGE_MAXLEN, 0)))
  {
    if (errno == EWOULDBLOCK || errno == EAGAIN)
    {
      return ""; // Timed out, it's OK
    }
    throw std::runtime_error("recv() failed"); // This is not OK
  }
  else if (bytes_recvd == 0)
    throw std::logic_error("recv() called but connection already terminated.");
  return std::string(buffer, bytes_recvd);
}

void Receiver::receive(Session &session)
{
  stopped = false;
  while(true)
  {
    try
    {
      received_data.append(receive_inner());
    }
    catch (std::exception &ex)
    {
      session.receiver_notify_failed(ex);
      return;
    }

    if (!received_data.empty())
    {
      ResponseParser response_parser = ResponseParser(received_data);

      std::unique_ptr<Response> response;
      try
      {
        while(response_parser.parse_next(response))
        {
          session.notify_incoming(std::move(response));
        }
      }
      catch(std::exception &ex)
      {
        session.receiver_notify_failed(ex);
      }

      received_data = "";
    }
    else
    {
      if (stopped)
      {
        break;
      }
    }
  }
}
