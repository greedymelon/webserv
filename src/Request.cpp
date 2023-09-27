#include "../inc/Request.hpp"
#include <string.h>
#include <stdlib.h>

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

Request::Request( unsigned int MaxBody ): _is_header_finish(0), _is_first_line(0),_env(NULL), _max_body_size(MaxBody){}
Request::~Request(void){
	if (_env == NULL)
		return ;
	for (int i = 0; i < 33; i++)
		free(_env[i]);
	free(_env);
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
	_body.append(_buffer);
	_buffer.clear();
	if (_body.size() > _max_body_size)
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
	if (st_check_method(_method) == METHOD_NOT_ALLOW)
	return 405;
	_buffer.erase(0, _buffer.find_first_of(' ') + 1);
	std::cout << _buffer<< std::endl << std::endl;
	_uri = _buffer.substr(0, _buffer.find_first_of(' '));
	_file_name = _uri;
	_query_string = "";
	if (_uri.find('?') != std::string::npos)
	{
		_query_string = _uri.substr(_uri.find('?'), _uri.length());
		_file_name = _uri.substr(0, _uri.find('?'));
	}
	_buffer.erase(0, _buffer.find_first_of(' ') + 1);
	return parse_protocol();
}

void Request::set_map(void)
{
	_is_header_finish = 0;
	// check if there some value in the string
	if (_buffer.find_first_of('\n') == std::string::npos)
		return ;
	std::string value;

	while (_buffer.find_first_of('\n') != 0)
	{
		//checking if part of previus key
		while (_buffer.find_first_of(':') == std::string::npos || _buffer.find_first_of(':') > _buffer.find_first_of('\n'))
		{
			value = _buffer.substr(_buffer.find_first_not_of(" \t"), _buffer.find_first_of("\r\n"));
				_buffer.erase(0 ,_buffer.find_first_of('\n') + 1);
				if((_header[ _last_key]).find_last_of(',') != (_header[ _last_key]).length())
				_header[ _last_key].append(", ");
			_header[ _last_key].append(value);
		}
		if (_buffer.find_first_of('\n') != 0)
			break ;
		_last_key = _buffer.substr(0, _buffer.find_first_of(':'));
		_buffer.erase(0,_buffer.find_first_of(':') + 2);
		if (_buffer.find_first_of("\r\n") != std::string::npos)
			value = _buffer.substr(0, _buffer.find_first_of("\r\n"));
		else
			value = _buffer.substr(_buffer.find_first_not_of(" \t"), _buffer.find_first_of('\n'));
		_buffer.erase(0,_buffer.find_first_of('\n') + 1);
		_header[ _last_key] =  value;
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
	if (_is_header_finish)
	{   
		if (_header.find("Transfer-Encoding") == _header.end())
		{ 
			else if (_max_body_size > unsigned(stoi(_header["Content-Lenght"])))
				_max_body_size = stoi(_header["Content-Lenght"]); 
		}
	}
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

void Request::create_env(void)
{
	_env = (char **)malloc(sizeof(char *) * 33);
	_env[32] = NULL;
	_env[0] = joing_string("COMSPEC=","");
	_env[1] = joing_string("DOCUMENT_ROOT=","");
	_env[2] = joing_string("GATEWAY_INTERFACE=", "CGI/1.1");
	_env[3] = joing_string("HOME=","");
	_env[4] = joing_string("HTTP_ACCEPT=","");
	_env[5] = joing_string("HTTP_ACCEPT_CHARSET=","");
	_env[6] = joing_string("HTTP_ACCEPT_ENCODING=","");
	_env[7] = joing_string("HTTP_ACCEPT_LANGUAGE=","");
	_env[8] = joing_string("HTTP_CONNECTION=","");
	_env[9] = joing_string("HTTP_HOST=","");
	_env[10] = joing_string("HTTP_USER_AGENT=","");
	_env[11] = joing_string("PATH=","");
	_env[12] = joing_string("PATHEXT=","");
	_env[13] = joing_string("PATH_INFO=","");
	_env[14] = joing_string("PATH_TRANSLATED=","");
	_env[15] = joing_string("QUERY_STRING=",_query_string.c_str());
	_env[16] = joing_string("REMOTE_ADDR=","");
	_env[17] = joing_string("REMOTE_PORT=","");
	_env[18] = joing_string("REQUEST_METHOD=", _method.c_str());
	_env[19] = joing_string("REQUEST_URI=", _uri.c_str());
	_env[20] = joing_string("SCRIPT_FILENAME=","DOCUMENT_ROOT+filename");
	_env[21] = joing_string("SCRIPT_NAME=",_file_name.c_str());
	_env[22] = joing_string("SERVER_ADDR=","");
	_env[23] = joing_string("SERVER_ADMIN=","");
	_env[24] = joing_string("SERVER_NAME=","");
	_env[25] = joing_string("SERVER_PORT=","");
	_env[26] = strdup("SERVER_PROTOCOL=HTTP/1.1");
	_env[27] = joing_string("SERVER_SIGNATURE=","");
	_env[28] = joing_string("SERVER_SOFTWARE=","");
	_env[29] = joing_string("SYSTEMROOT=","");
	_env[30] = joing_string("TERM=","");
	_env[31] = joing_string("WINDIR=","");
}

char *const *Request::get_env(void) const
{	
	return (_env);
}

std::string Request::get_file_addr(void) const
{
	return _file_name;
}

int	Request::is_complete_request(void)
{
	if (_is_header_finish && _is_first_line)
	{
		create_env();
		return 0;
	}
	return 400;
}

const char	*Request::get_dir( void ) const
{
	return _uri.substring(0, _uri.find_last_of("/")).cstr();
}