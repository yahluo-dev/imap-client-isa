/**
 * @file imf_message.hpp
 * @brief A class implementing parsing of some components of a message in IMF format
 * @author Iaroslav Vasilevskii (xvasil10)
 */
#ifndef IMF_MESSAGE_H_
#define IMF_MESSAGE_H_
#include <string>

class IMFMessage
{
  private:
  std::string data;
  public:
  std::string get_datetime_formatted() const;
  std::string get_from_formatted() const;
  std::string get_data() const;
  IMFMessage(const std::string &_data)
    : data(_data){};
};

#endif // IMF_MESSAGE_H_
