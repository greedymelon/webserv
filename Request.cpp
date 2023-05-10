#include "Request.hpp"


Request::Request(void):_method(0),_is_first_line(0), _is_header_finish(0){}
Request::~Request(void){}


int st_select_method(std::string method)
{
    if (method.compare("GET"))
        return GET;
    if (method.compare("POST"))
        return POST;
    if (method.compare("DELETE"))
        return DELETE;
    return METHOD_NOT_ALLOW;
}
int Request::set_body(void)
{
    _body.append(_buffer);
    _buffer.clear();
    if (_body.size() > atol(get_info("Content-Lenght").c_str()))
        return (400);
    return 0;
}

int  Request::parse_protocol(void)
{
    
}

int  Request::set_MetAddProt(void)
{
       if (_buffer.find_first_of('\n') == std::string::npos)
        return 1;
    _method = st_select_method(_buffer.substr(0, _buffer.find_first_of(" ")));
    if (_method == METHOD_NOT_ALLOW)
        return 405;
    _buffer.erase(0, _buffer.find_first_of(" ") + 1);
    _address = _buffer.substr(0, _buffer.find_first_of(" "));
    _buffer.erase(0, _buffer.find_first_of(" ") + 1);
    return parse_protocol();
}

void Request::set_map(void)
{
    if (_buffer.find_first_of('\n') == std::string::npos)
        return ;
    while (_buffer.find_first_of('\n') != 0)
    {
        std::string key = _buffer.substr(0, _buffer.find_first_of(":"));
        _buffer.erase(0,_buffer.find_first_of(":") + 2);
        std::string value = _buffer.substr(0, _buffer.find_first_of("\n"));
         _buffer.erase(0,_buffer.find_first_of("\n") + 1);
        _header[key] =  value;
        if (_buffer.find_first_of('\n') == std::string::npos)
            return ;
    }
    _is_header_finish = 1;
    _buffer.erase(0, 1);
}

int Request::feed(std::string chunk)
{
    int result = 0;
    _buffer.append(chunk);
    if (!_is_first_line)
        result = set_MetAddProt();
    if (_is_first_line && !_is_header_finish && !result)
        set_map();
    if (_is_first_line && _is_header_finish && !result)
        return (set_body());
    return (result)
}