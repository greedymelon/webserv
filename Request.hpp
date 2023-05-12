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

            int         _method;
            std::string _address;
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
            int feed(std::string chunk);
            std::string get_address(void) const;
            std::string get_info(std::string key) const;
            bool is_info_present(std::string key) const;
            int get_method(void) const;
            std::string get_prothocol(void);

};

typedef enum e_request_type
{
    GET,
    POST,
    DELETE,
    METHOD_NOT_ALLOW = 405,
    WRONG_PROTOCOL = ,
} t_request_type;

#endif
