#include <string>
#include <vector>
#include <stdint.h>

#ifndef COMMAND_H_
#define COMMAND_H_

enum command_type_t
{
  COMMAND_LOGIN,
  COMMAND_SELECT,
  COMMAND_SEARCH,
  COMMAND_FETCH,
};

class Command
{
  protected:
    command_type_t type;
    std::string tag;
  public:
    Command(std::string tag);
    command_type_t get_type();
    virtual std::string make_tcp();
    virtual ~Command() = default;
};

class LoginCommand: public Command
{
  private:
  std::string user_name;
  std::string password;
  public:
  explicit LoginCommand(std::string _tag, std::string _user_name, std::string _password);
  std::string make_tcp() override;
};

class SelectCommand: public Command
{
  private:
  std::string mailbox_name;
  public:
  explicit SelectCommand(std::string _tag, std::string _mailbox_name);
  std::string make_tcp() override;
};

class SearchCommand: public Command
{
  private:
  std::string search_criteria;
  public:
  explicit SearchCommand(std::string _tag, std::string _search_criteria);
  std::string make_tcp() override;
};

class FetchCommand: public Command
{
  private:
  std::vector<uint32_t> sequence_set;
  std::string data_item;
  public:
  explicit FetchCommand(std::string _tag, std::vector<uint32_t> sequence_set, std::string _data_item);
  std::string make_tcp() override;
};

class StoreCommand : public Command
{
  private:
  std::vector<uint32_t> sequence_set;
  std::string data_item_name;
  std::string data_item_value;
  public:
  explicit StoreCommand(std::string _tag, std::vector<uint32_t> sequence_set,
                        std::string _data_item_name, std::string _data_item_value);
  std::string make_tcp() override;
};

class LogoutCommand : public Command
{
  private:
  public:
  explicit LogoutCommand(std::string _tag);
  std::string make_tcp() override;
};

#endif // COMMAND_H_
