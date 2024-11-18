#ifndef DIRECTORY_WRITER_H_
#define DIRECTORY_WRITER_H_

#include <vector>
#include <string>
#include <fstream>
#include "imf_message.hpp"
#include "logger.hpp"

class DirectoryWriter
{
  private:
  Logger logger;
  std::string output_dir;
  public:
  DirectoryWriter(std::string output_dir);
  void save_messages(std::vector<IMFMessage> messages);
};

#endif // DIRECTORY_WRITER_H_
