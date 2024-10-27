#include "logger.hpp"

void Logger::debug_log( std::string message)
{
  if (log_level > LogLevel::DEBUG)
    return;
  out_stream << "DEBUG :: " << message << std::endl;
}

void Logger::error_log( std::string message)
{
  if (log_level > LogLevel::ERROR)
    return;
  out_stream << "ERROR :: " << message << std::endl;
}

void Logger::info_log( std::string message)
{
  if (log_level > LogLevel::INFO)
    return;
  out_stream << "INFO :: " << message << std::endl;
}
