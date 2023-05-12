#include "Request.hpp"


Request::Request(void):_method(0),_is_first_line(0), _is_header_finish(0){}
Request::~Request(void){}


int st_select_method(std::string method)
{
    if (method.compare("GET") == 0)
        return GET;
    if (method.compare("POST") == 0)
        return POST;
    if (method.compare("DELETE") == 0)
        return DELETE;
    return METHOD_NOT_ALLOW;
}

int Request::set_body(void)
{
    if (_method == GET)
        return (400);
    if ( _header.find("Content-Lenght") == _header.end())
        return (401);
    _body.append(_buffer);
    _buffer.clear();
    // if (_body.size() > conf.maxbodysie)
    //     return (413);
    if (_body.size() > atol(get_info("Content-Lenght").c_str()))
        return (400);
    return 0;
}

int  Request::parse_protocol(void)
{
    if (_buffer.find_first_of("HTTP/1.1")  == std::string::npos)
        return WRONG_PROTOCOL;
    _protocol = "HTTP/1.1";
    _buffer.erase(0, _buffer.find_first_of('\n') + 1);
    _is_first_line = 1;
    return 0;
}

int  Request::set_MetAddProt(void)
{
    if (_buffer.find_first_of('\n') == std::string::npos)
        return 1;
    _method = st_select_method(_buffer.substr(0, _buffer.find_first_of(' ')));
    if (_method == METHOD_NOT_ALLOW)
        return 405;
    _buffer.erase(0, _buffer.find_first_of(' ') + 1);
    _address = _buffer.substr(0, _buffer.find_first_of(' '));
    _buffer.erase(0, _buffer.find_first_of(' ') + 1);
    return parse_protocol();
}

void Request::set_map(void)
{
     _is_header_finish = 0;
    if (_buffer.find_first_of('\n') == std::string::npos)
        return ;
    std::string value;
    while (_buffer.find_first_of("\r\n") != 0 && _buffer.find_first_of('\n') != 0)
    {
        std::string key = _buffer.substr(0, _buffer.find_first_of(':'));
        _buffer.erase(0,_buffer.find_first_of(':') + 2);
        if (_buffer.find_first_of("\r\n") != std::string::npos)
            value = _buffer.substr(0, _buffer.find_first_of("\r\n"));
        else
            value = _buffer.substr(0, _buffer.find_first_of('\n'));
        _buffer.erase(0,_buffer.find_first_of('\n') + 1);
        _header[key] =  value;
        if (_buffer.find_first_of('\n') == std::string::npos)
            return ;
    }
    _is_header_finish = 1;
    if (_buffer.find_first_of("\r\n") == std::string::npos)
        _buffer.erase(0, 1);
    else
        _buffer.erase(0, 2);
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
    return result ;
}

std::string Request::get_address(void) const
{
    return _address;
}

std::string Request::get_info(std::string key) const
{
    return _header.find(key)->second;
}

bool  Request::is_info_present(std::string key) const
{
    if ( _header.find(key) == _header.end())
        return false;
    return true;

}

int Request::get_method(void) const
{
    return _method;
}
std::string Request::get_protocol(void) const
{
    return _protocol;
}

std::string Request::get_body(void) const
{
    return _body;
}