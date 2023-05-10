#ifndef RESPONSE_HPP
# define RESPONCE_HPP
# include <string>

class Response
{
    private:
        std::string _response;
    public:
        Response(void);
        Response(int error);
        ~Response (void);
        std::string get_response(void) const;
        void set_response(int error);
        void set_responce(std::string response);


};
#endif