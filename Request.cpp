#include "Request.hpp"

// const char *param[] =
// {
//     "COMSPEC=",
//     "DOCUMENT_ROOT=",
//     "GATEWAY_INTERFACE=",
//     "HOME=",
//     "HTTP_ACCEPT=",
//     "HTTP_ACCEPT_CHARSET=",
//     "HTTP_ACCEPT_ENCODING=",
//     "HTTP_ACCEPT_LANGUAGE=",
//     "HTTP_CONNECTION=",
//     "HTTP_HOST=",
//     "HTTP_USER_AGENT=",
//     "PATH=",
//     "PATHEXT=",
//     "PATH_INFO=",
//     "PATH_TRANSLATED=",
//     "QUERY_STRING=",
//     "REMOTE_ADDR=",
//     "REMOTE_PORT=",
//     "REQUEST_METHOD=",
//     "REQUEST_URI=",
//     "SCRIPT_FILENAME=",
//     "SCRIPT_NAME=",
//     "SERVER_ADDR=",
//     "SERVER_ADMIN=",
//     "SERVER_NAME=",
//     "SERVER_PORT=",
//     "SERVER_PROTOCOL=HTTP/1.1",
//     "SERVER_SIGNATURE=",
//     "SERVER_SOFTWARE=",
//     "SYSTEMROOT=",
//     "TERM=",
//     "WINDIR="
// };

Request::Request(void): _is_header_finish(0), _is_first_line(0), _method(0){}
Request::~Request(void){
    for (int i = 0; argv[i]; i++)
        free(argv[i]);
    free(*argv);
}


int st_check_method(std::string method)
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
    if (_method.compare("GET") == 0)
        return (400);
    if ( _header.find("Content-Lenght") == _header.end())
        return (401);
    _body.append(_buffer);
    _buffer.clear();
    // if (_body.size() > conf.maxbodysie)
    //     return (413);
    if (_body.size() > (unsigned long)atol(get_info("Content-Lenght").c_str()))
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
    _method  = _buffer.substr(0, _buffer.find_first_of(' '));
    if (st_check_method(_method)== METHOD_NOT_ALLOW)
        return 405;
    _buffer.erase(0, _buffer.find_first_of(' ') + 1);
    _uri = _buffer.substr(0, _buffer.find_first_of(' '));
    _is_cgi = true;
    if (_uri.find('?') == std::string::npos)
        _is_cgi = false;
    if (_is_cgi)
    {    
        _cgi = _uri.substr(_uri.find('?'), _uri.length());
        _uri = _uri.substr(0, _uri.find('?'));
    }
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

int Request::feed(const char *chunk)
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

std::string Request::get_uri(void) const
{
    return _uri;
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
     if (_method.compare("GET") == 0)
        return GET;
    if (_method.compare("POST") == 0)
        return POST;
    if (_method.compare("DELETE") == 0)
        return DELETE;
}
std::string Request::get_protocol(void) const
{
    return _protocol;
}

const char *Request::get_body(void) const
{
    return _body.c_str();
}


size_t Request::get_bodySize(void) const
{
    return _body.length();
}

static char * joing_string(const char *str1, const char *str2)
{
    size_t size = strlen(str1) + strlen(str2);
    char *string = (char *)malloc((size + 1) * sizeof(char));
    strcpy(string, str1);
    strcat(string, str2);
    string[size] = '\0';
    return (string);
}

void Request::create_argv(void)
{
    argv = (char **)malloc(sizeof(char *) * 33);
    argv[32] = strdup("");
    argv[0] = joing_string("COMSPEC=","");
    argv[1] = joing_string("DOCUMENT_ROOT=","");
    argv[2] = joing_string("GATEWAY_INTERFACE=","");
    argv[3] = joing_string("HOME=","");
    argv[4] = joing_string("HTTP_ACCEPT=","");
    argv[5] = joing_string("HTTP_ACCEPT_CHARSET=","");
    argv[6] = joing_string("HTTP_ACCEPT_ENCODING=","");
    argv[7] = joing_string("HTTP_ACCEPT_LANGUAGE=","");
    argv[8] = joing_string("HTTP_CONNECTION=","");
    argv[9] = joing_string("HTTP_HOST=","");
    argv[10] = joing_string("HTTP_USER_AGENT=","");
    argv[11] = joing_string("PATH=","");
    argv[12] = joing_string("PATHEXT=","");
    argv[13] = joing_string("PATH_INFO=","");
    argv[14] = joing_string("PATH_TRANSLATED=","");
    argv[15] = joing_string("QUERY_STRING=",_cgi.c_str());
    argv[16] = joing_string("REMOTE_ADDR=","");
    argv[17] = joing_string("REMOTE_PORT=","");
    argv[18] = joing_string("REQUEST_METHOD=", _method.c_str());
    argv[19] = joing_string("REQUEST_URI=", _uri.c_str());
    argv[20] = joing_string("SCRIPT_FILENAME=","");
    argv[21] = joing_string("SCRIPT_NAME=","");
    argv[22] = joing_string("SERVER_ADDR=","");
    argv[23] = joing_string("SERVER_ADMIN=","");
    argv[24] = joing_string("SERVER_NAME=","");
    argv[25] = joing_string("SERVER_PORT=","");
    argv[26] = strdup("SERVER_PROTOCOL=HTTP/1.1");
    argv[27] = joing_string("SERVER_SIGNATURE=","");
    argv[28] = joing_string("SERVER_SOFTWARE=","");
    argv[29] = joing_string("SYSTEMROOT=","");
    argv[30] = joing_string("TERM=","");
    argv[31] = joing_string("WINDIR=","");
}

char *const *Request::get_argv(void) const
{
    create_argv();
    return (argv);
}

bool Request::is_cgi(void) const
{
    return _is_cgi;
}