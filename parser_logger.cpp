/**
 * @file parser_logger.cpp
 * @brief Logger subclass for the recursive descent parser
 * @author Iaroslav Vasilevskii (xvasil10)
 */
#include "parser_logger.hpp"

void ParserLogger::parser_debug_log(int recursion_level, size_t position,
                      const std::string function_name,
                      const std::string message)
{
  if (log_level > LogLevel::DEBUG)
    return;
  std::string data_char;
  if (data[position] == '\n')
    data_char = "\\n";
  else if (data[position] == '\r')
    data_char = "\\r";
  else
    data_char = data[position];
  out_stream << "DEBUG :: " << std::string(recursion_level, '.') << "->" << function_name <<  " at char. " << position << "(" << data_char << "): " << message << std::endl;
}
