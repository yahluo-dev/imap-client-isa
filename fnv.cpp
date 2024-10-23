#include "fnv.hpp"
#include <iomanip>

std::string FNV::hash(const std::string &data)
{
  uint64_t hash = FNV_OFFSET_BASIS;
  std::stringstream result;

  for (char c : data)
  {
    hash ^= static_cast<uint64_t>(c);
    hash *= FNV_PRIME;
  }

  result << std::hex << std::setw(8) << std::setfill('0') << hash;

  return result.str();
}
