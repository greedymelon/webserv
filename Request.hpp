#ifndef REQUEST_HPP
# define REQUEST_HPP
# include <string>

class Request
{
    
        private:
		    Request(void );
            std::string head; 
            std::string body;

        public:
            Request( std::string head, std::string body);
		    ~Request( void );
            const std::string get_head() const;
            const std::string get_body() const;
};




#endif