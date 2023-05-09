#include "RequestHandler.hpp"
#include <cstring>
#include <iostream>

void RequestHandler::set_method(std::string first_line)
{
    std::string address = first_line.substr(first_line.find_first_of("/"), first_line.find_last_of(" ") - first_line.find_first_of("/"));
    address = address.substr(0, address.find_first_of(" "));
    this->_page = address.c_str();
} 

RequestHandler::RequestHandler(Configuration &data, Request &request):_conf(data), _clientRequest(request), _response(new Response)
{
    set_response();
}

RequestHandler::~RequestHandler(void){}

Response *RequestHandler::get_response() const
{
    return (this->_response);
}

void RequestHandler::set_response(void)
{
    std::string first_line;
    std::getline(_clientRequest.get_fd(), first_line);
    set_method(first_line);
    set_page(first_line);
    switch(this->_method)
    {
        case GET:
            set_get();
            break;
        case POST:
            set_post();
            break;
        case DELETE:
            set_delete();
            break;
        default:
            set_error();
    }
}
void RequestHandler::set_method(std::string first_line)
{
    if (first_line.find_first_of("HTTP/1.1") == std::string::npos)
    {
        _method = WRONG_PROTOCOL;
        return ;
    }
    if (first_line.find_first_of("GET ") == 0)
        _method = GET;
    else if (first_line.find_first_of("POST ") == 0)
        _method = POST;
    else if (first_line.find_first_of("DELETE ") == 0)
        _method = DELETE;
    else
        _method = METHOD_NOT_ALLOW;
    
}
void RequestHandler::set_get(void)
{
    std::ifstream ifs;
    std::ofstream ofs;

    ifs.open(_page);
    ofs.open("fakeRespose");
    ofs << "HTTP/1.1 200 OK" << std::endl;



}
