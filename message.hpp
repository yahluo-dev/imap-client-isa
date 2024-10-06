#ifndef MESSAGE_H_
#define MESSAGE_H_

#include <string>

class IMFMessage
{
  public:
  std::string from;
  std::string to;
  std::string subject;
  std::string message_id;
  std::string body;
  IMFMessage(std::string _from, std::string _to, std::string _subject, std::string _message_id, std::string _body)
    : from(_from), to(_to), subject(_subject), message_id(_message_id), body(_body) {};
};

#endif // MESSAGE_H_
