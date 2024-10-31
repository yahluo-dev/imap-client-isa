#ifndef PARSER_LOGGER_H
#define PARSER_LOGGER_H

#include "logger.hpp"

class ParserLogger : public Logger
{
  private:
  std::string &data;
  public:
  ParserLogger(std::ostream &_out_stream, std::string &_data)
    : Logger(_out_stream), data(_data){};
  ParserLogger(std::ostream &_out_stream, std::string &_data, LogLevel log_level)
    : Logger(_out_stream, log_level), data(_data){};
  void parser_debug_log(int recursion_level, size_t position,
                        const std::string function_name,
                        const std::string message);
};

#endif // PARSER_LOGGER_H
