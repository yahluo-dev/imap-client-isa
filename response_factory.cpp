#include <stdexcept>
#include "response_factory.hpp"

std::unique_ptr<Response> ResponseFactory::create(const std::string message)
{
  throw std::logic_error("Not implemented");
  return std::make_unique<Response>("");
}
