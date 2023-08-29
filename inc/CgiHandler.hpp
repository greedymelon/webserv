#ifndef CGIHANDLER_HPP
# define CGIHANDLER_HPP
# include <string>
# include <vector>
# include "Request.hpp"
# include "Configuration.hpp"
# include "Response.hpp"

class CgiHandler
{
    private:
        std::vector<uint8_t *> _image;
        std::vector<char *> _page;



    public:
        //CgiHandler(Request &request);
        ~CgiHandler( void );
        CgiHandler( void );
        void feed(const char *chunk);
        void feed(const uint8_t *chunk);
        Response get_response() const;
};


#endif
