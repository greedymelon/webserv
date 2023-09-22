#include "Request.hpp"
#include "CgiHandler.hpp"
#include <iostream>
#include <stdlib.h>
#include <unistd.h>

int main()
{
	// first we need to create a request
	Request request;

	//than we feed our request with a char *
	// the request request return 0 if evertythin is write or the error
	// that corrispond to the server error in case enconter an error
	int error;
	error = request.feed("POST /hello.htm HTTP/1.1\n");
	//check if the request is finished
	std::cout << request.is_complete_request();
	//retrive usefull information
	request.get_uri();
	request.get_method();
	request.get_body();
	request.get_env();
	request.is_cgi();
	request.get_script_addr();

	// if it is cgi
	CgiHandler cgi(request.get_script_addr(), request.get_env(), request.get_body());
	// to get the status
	int status = cgi.get_status_code();
	// if status not 200 there would not be an output to send back to the user
	cgi.get_response();
}
