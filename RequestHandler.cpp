#include "RequestHandler.hpp"
#include <cstring>
#include <iostream>

RequestHandler::RequestHandler(Configuration &data, Request *request):_conf(data), _clientRequest(request)
{
    //start process
}


RequestHandler::RequestHandler(Configuration &data, int error):_conf(data)
{
    //if error page not in confg
}

Response RequestHandler::get_response() const
{
    return _response;
}