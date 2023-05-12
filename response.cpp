#include "Response.hpp"

Response::Response(){}
Response::~Response(){
    delete _response;
}

std::string *Response::get_response(void) const
{
    return _response;
}

void Response::set_response(std::string *response)
{
    _response = response;
}

void Response::set_error(int error)
{
    //generate http personalize error pag
}
