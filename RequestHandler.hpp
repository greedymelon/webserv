#ifndef REQUESTHANDLER_HPP
# define REQUESTHANDLER_HPP
# include <string>
# include "Request.hpp"
# include "Configuration.hpp"
# include "Response.hpp"

class RequestHandler
{
    private:
        Request &_client;
        Response *_response;
        Configuration &_data;
        int _method;
        void parse_header();
        void set_method();
        void set_get();
        void set_post();
        void set_delete();
        void set_error();
        int read_requestHead() const;
        void set_response();

    public:
        RequestHandler(Configuration &data, Request &client);
        ~RequestHandler();
        Response *get_response() const;
};




#endif