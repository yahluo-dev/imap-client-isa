#ifndef RESPONSE_FACTORY_H_
#define RESPONSE_FACTORY_H_

#include <memory>

#include "response.hpp"

class ResponseFactory
{
  public:
  static std::unique_ptr<Response> create(const std::string message);
};

#endif // RESPONSE_FACTORY_H_
