#ifndef REQUESTHANDLER_HPP
# define REQUESTHANDLER_HPP
# include <string>
# include "Request.hpp"
# include "Configuration.hpp"
# include "Response.hpp"

class RequestHandler
{
    private:
            Request *_clientRequest;
            Response _response;
            Configuration &_conf;


    public:
        RequestHandler(Configuration &data, Request *request);
        RequestHandler(Configuration &data, int error);
        ~RequestHandler();
        Response get_response() const;
};


#endif
