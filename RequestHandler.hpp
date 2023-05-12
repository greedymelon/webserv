#ifndef REQUESTHANDLER_HPP
# define REQUESTHANDLER_HPP
# include <string>
# include "Request.hpp"
# include "Configuration.hpp"
# include "Response.hpp"

class RequestHandler
{
    private:
        Request &_clientRequest;
        Response *_response;
        Configuration &_conf;
        std::string _protocol;
        std::string _connection;
        std::string _host;
        const char *_page;
        int _method;
        void parse_header();
        void set_method(std::string first_line);
        void set_page(std::string first_line);
        void set_get();
        void set_post();
        void set_delete();
        void set_error();
        int read_requestHead() const;
        void set_response();

    public:
        RequestHandler(Configuration &data, Request &request);
        ~RequestHandler();
        Response *get_response() const;
};


#endif
