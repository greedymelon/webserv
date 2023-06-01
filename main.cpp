#include "RequestHandler.hpp"
#include <iostream>

int main()
{
    Request request;

    if (request.feed("POST /hello.html HTTP/1.1\n") == 0)
    {   
        std::cout<<"method num: " << request.get_method(); 
        std::cout << " address: " << request.get_uri();
        std::cout << " protocol: " << request.get_protocol() << std::endl;
    }
    else
    {
        std::cout<< "error" << std::endl;
    }
    if (request.feed("User-Agent: Mozilla/4.0 (compatible; MSIE5.01; Windows NT)\r\nHost: www.tutorialspoint.com\r\nContent-Lenght: 30\r\n") == 0)
    {   
        std::cout<<"1 line: " << request.get_info("User-Agent")<< std::endl;
        std::cout << "2 line: " << request.get_info("Host")<< std::endl;
        std::cout << "3 line: " << request.get_info("Content-Lenght")<< std::endl;
    }
    else
    {
        std::cout<< "error" << std::endl;
    }
    if (request.feed("\n bodydssdsdsdsd") == 0)
    {   
        std::cout<<"body: " << request.get_body()<< std::endl;
    }
    else
    {
        std::cout<< request.feed("\n bodydssdsdsdsd") << std::endl;
    }

    Request request2;

    if (request2.feed("POST /hello.py?abc=123&xyz=987 HTTP/1.1\n") == 0)
    {   

        std::cout<<"method num: " << request2.get_method() << std::endl; 
        std::cout << "address: " << request2.get_uri() << std::endl;
        std::cout<<"is sgi: " << request2.is_cgi()<< std::endl;
        if ( request2.is_cgi())
            std::cout<<"cgi: " << request2.get_cgi() << std::endl;
        else
            std::cout<< "no cgi " << std::endl;
        std::cout << "protocol: " << request2.get_protocol() << std::endl;
    }

}