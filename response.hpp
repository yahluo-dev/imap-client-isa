#ifndef RESPONSE_H_
#define RESPONSE_H_

enum response_type_t
{
  RESPONSE_OK,
  RESPONSE_NO,
  RESPONSE_BAD,
  RESPONSE_PREAUTH,
  RESPONSE_BYE,
  RESPONSE_SEARCH,
  RESPONSE_FETCH,
};

class Response
{
  protected:
  response_type_t type;
  public:
  response_type_t get_type();
};

#endif // RESPONSE_H_
