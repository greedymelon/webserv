#ifndef RESPONSE_HPP
# define RESPONCE_HPP
# include <string>

class Response
{
    private:
        std::string *_head;
        std::string *_body;
    public:
        Response(void);
        ~Response (void);
        void   set_head(std::string head);
        void    set_body(std::string head);
        std::string get_head(void) const;
        std::string get_body(void) const;
};
#endif