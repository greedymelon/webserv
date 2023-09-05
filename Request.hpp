#ifndef REQUEST_HPP
# define REQUEST_HPP
# include <string>
# include <iostream>
# include <fstream>
# include <map>

class Request
{
    
        private:
            bool    _is_header_finish;
            bool    _is_first_line;
            std::string _buffer;
            std::string _last_key;

            std::string      _method;
            bool        _is_cgi;
            char **_argv;
            std::string _cgi;
            std::string _uri;
            std::string _protocol;
            std::map<std::string, std::string> _header;
            std::string _body;

            int set_MetAddProt(void);
            int parse_protocol(void);
            void set_map(void);
            int set_body(void);
            
            
        public:
            Request(void);
		    ~Request( void );
            int         feed(const char* chunk);
            std::string get_uri(void) const;
            std::string get_info(std::string key) const;
            bool        is_info_present(std::string key) const;
            int         get_method(void) const;
            std::string get_protocol(void) const;
            const char  *get_body(void) const;
            size_t      get_bodySize(void) const;
            void create_argv(void);

            char *const *get_argv(void) const;
            bool        is_cgi(void) const;

};


typedef enum e_request_type
{
    GET,
    POST,
    DELETE,
    METHOD_NOT_ALLOW = 405,
    WRONG_PROTOCOL = 505,
} t_request_type;

#endif
