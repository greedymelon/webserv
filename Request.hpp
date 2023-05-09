#ifndef REQUEST_HPP
# define REQUEST_HPP
# include <string>
# include <iostream>
# include <fstream>

class Request
{
    
        private:
		    Request(void );
            std::ifstream _request;
        public:
            Request(std::ifstream  fd);
		    ~Request( void );
            std::ifstream get_fd() const;
};



#endif
