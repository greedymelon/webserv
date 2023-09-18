#ifndef CGIHANDLER_HPP
# define CGIHANDLER_HPP
# include <string>
# include <vector>
# include "Request.hpp"

class CgiHandler
{
    private:
        int _fd_in[2];
        int _fd_out[2];
        int _fd_err[2];
        int _response_num;
        pid_t _child_id;
        std::string _response;
        std::string _response_err;
        std::string _err;
        int initialize_pipe(void);
        void child_exe(const char *script, char * const *env);
        void parent_exe(const char *body);

    public:
        CgiHandler(const char *script, char * const *env, const char *body);
        ~CgiHandler(void);
        int get_response_num (void) const;
        std::string get_error (void) const;
        std::string get_response (void) const;

};


#endif
