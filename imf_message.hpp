#ifndef IMF_MESSAGE_H_
#define IMF_MESSAGE_H_
#include <string>

class IMFMessage
{
  private:
  std::string data;
  public:
  std::string get_datetime_formatted();
  std::string get_from_formatted();
  IMFMessage(const std::string &_data)
    : data(_data){};
};

#endif // IMF_MESSAGE_H_
