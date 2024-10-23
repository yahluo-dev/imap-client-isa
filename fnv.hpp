#ifndef FNV_H_
#define FNV_H_

#include <string>
#include <stdint.h>


class FNV // Fowler-Noll-Vo hash function
{
  private:
  const uint64_t FNV_OFFSET_BASIS = 0xcbf29ce484222325;
  const uint64_t FNV_PRIME = 0x100000001b3;
  public:
  std::string hash(const std::string &data);
};

#endif // FNV_H_
