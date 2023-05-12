#ifndef RESPONSE_HPP
# define RESPONSE_HPP
# include <string>

class Response
{
    private:
        std::string *_response;
    public:
        Response(void);
        ~Response (void);
        std::string *get_response(void) const;
        void set_response(std::string *response);
        void set_error(int error);


};
#endif