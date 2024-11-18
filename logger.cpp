/**
 * @file logger.cpp
 * @brief Class implementing logging capabilities with adjustable logging level
 * @author Iaroslav Vasilevskii (xvasil10)
 */
#include <iostream>
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
  std::cout << "INFO :: " << message << std::endl; // Info always printed to stdout
}
