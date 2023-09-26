#include "inc/Request.hpp"
#include "inc/CgiHandler.hpp"
#include <iostream>
#include <stdlib.h>
#include <unistd.h>
#define MAX_BODY_SIZE 1000

int main()
{
	// first we need to create a request
	Request request(MAX_BODY_SIZE);

	//than we feed our request with a char *
	// the request request return 0 if evertythin is write or the error
	// that corrispond to the server error in case enconter an error
	int error;
	error = request.feed("POST /hello.htm HTTP/1.1\n");

	//check if the request is finished
	std::cout << request.is_complete_request();
	//retrive usefull information

	//check the config file if the dir is there:
	request.get_dir();
	//check if the method is allow
	request.get_method();
	// then we would have to join the document root (ex in the conf: root www;) with the file requested also if it is a script
	// es: root.append(request.get_file_addr()) 
	std::string file_path;
	file_path.append("ROOT_COMING_FROM_CONF");
	file_path.append(request.get_file_addr());
	//check if it is cgi and then run
	CgiHandler cgi(file_path.c_str(), request.get_env(), request.get_body());
	std::string toClient;
	//check what is the exit code
	switch (cgi.get_status_code())
	{
	case 403:
		toClient = cgi.get_status_mess() + "Content-type:text/html\r\n\r\n" + "FILE_CONTENT";
		break;
	case 502:
		toClient = cgi.get_status_mess() + "Content-type:text/html\r\n\r\n" + "FILE_CONTENT";
		break;
	case 504:
		toClient = cgi.get_status_mess() + "Content-type:text/html\r\n\r\n" + "FILE_CONTENT";
		break;	
	default:
		toClient = cgi.get_status_mess() + cgi.get_response(); // 200
		break;
	}
}
