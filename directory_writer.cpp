/**
 * @file directory_writer.cpp
 * @brief Class for writing messages to an output directory with an appropriate file name
 * @author Iaroslav Vasilevskii (xvasil10)
 */
#include "directory_writer.hpp"
#include "fnv.hpp"
#include <iostream>
#include <stdexcept>


DirectoryWriter::DirectoryWriter(std::string _output_dir) : logger(std::cerr)
{
  output_dir = _output_dir;
}

void DirectoryWriter::save_messages(std::vector<IMFMessage> messages)
{
  FNV fnv;
  for (const IMFMessage& imf_message : messages)
  {
    std::string path = output_dir + "/" + fnv.hash(imf_message.get_data()) +
      imf_message.get_datetime_formatted() + ".eml";
    std::ofstream save_to(path);
    if (!save_to)
    {
      throw std::runtime_error("Could not open " + path + " for writing.");
      return;
    }

    save_to << imf_message.get_data() << std::endl;
    save_to.close();
  }
}
