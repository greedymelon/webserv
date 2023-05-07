#include "RequestHandler.hpp"


RequestHandler::RequestHandler(Configuration &data, Request *client):_data(data), _client(client), _response(new Response)
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
    set_method();
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