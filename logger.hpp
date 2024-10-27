#ifndef LOGGER_H_
#define LOGGER_H_

#include <fstream>

#ifndef NDEBUG
#define GLOBAL_LOG_LEVEL LogLevel::DEBUG
#else
#define GLOBAL_LOG_LEVEL LogLevel::ERROR
#endif // NDEBUG

enum class LogLevel
{
  DEBUG,
  INFO,
  ERROR
};

class Logger
{
  protected:
  std::ostream &out_stream;
  LogLevel log_level;
  public:
  Logger(std::ostream &_out_stream)
    : out_stream(_out_stream), log_level(GLOBAL_LOG_LEVEL){};
  Logger(std::ostream &_out_stream, LogLevel _log_level)
    : out_stream(_out_stream), log_level(_log_level){};
  void debug_log(std::string message);
  void error_log(std::string message);
  void info_log(std::string message);
};

#endif // LOGGER_H_
