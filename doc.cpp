#include "inc/Request.hpp"
#include "inc/CgiHandler.hpp"
#include <iostream>
#include <fstream>
#include <iostream>
#include <stdlib.h>
#include <unistd.h>
#include <sys/event.h>
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

	// in case is not cgi check if exist and if reading permission are there
	std::string response;
	if (access(file_path.c_str(), R_OK))
	{	
		response = "HTTP/1.1 200 OK\r\n";
	}
	else if (access(file_path.c_str(), F_OK))
	{	
		response = "HTTP/1.1 404 NOT FOUND\r\n";
		file_path = "404.html";
	}
	else
	{
		response = "HTTP/1.1 403 Forbidden\r\n";
		file_path = "403.html";
	}
	//for reading from a file
	response.append(read_file(file_path));
}

std::string	read_file (std::string file_path)
{
	std::string response;
	std:: string ext =  file_path.substr(file_path.find_last_of("."),file_path.size());
	if (ext == ".png")
		response = "Content-type:png\r\n\r\n";
	else if (ext == ".jpeg")
		response = "Content-type:jpeg\r\n\r\n";
	else if (ext == ".css")
		response = "Content-type:stylesheet\r\n\r\n";
	else
		response = "Content-type:text/html\r\n\r\n";
	std::ifstream my_file (file_path.c_str());
	fcntl(my_file, F_SETFL, O_NONBLOCK);
	int kque = kqueue();
	struct kevent event;
	EV_SET(&event, my_file, EVFILT_READ, EV_ADD, 0, 0, NULL);
	int read_finished == 0;
	while (1)
	{
		int num_events = kevent(kque, NULL, 0, event, 1, &timeout);
		for (int i = 0; i < num_events; i++) 
		{ 
			if (event[i].filter == EVFILT_READ)
			{
				if (my_file.eof())
				{	
					my_file.close();
					read_finished++;
					break ;
				}
				_response.append(my_file.rdbuf());
			}
		}
		if (read_finished)
			break ;
	}
	close(kque);
	return response;
}