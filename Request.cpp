#include "Request.hpp"


Request::Request(void):_method(0), _is_header_finish(0){}
Request::~Request(void){}

int Request::set_body(void)
{
    _body.append(_buffer);
    _buffer.clear();
    if (_body.size() > atol(get_info("Content-Lenght").c_str()))
        return (somthing);
    return 0;

}
int Request::feed(std::string chunk)
{
    _buffer.append(chunk);
    if (_is_header_finish)
        return (set_body());
    if (_method == 0 && _buffer.size() > 4)



}