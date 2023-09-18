#include "CgiHandler.hpp"
#include <cstring>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <string>
#include <signal.h>
#include <sys/wait.h>

pid_t pid;
static void st_sig_kill(int signum)
{
   if (signum)
      kill(pid, SIGKILL);
}


static void st_close_fd(int fd_1, int fd_2)
{
   close(fd_1);
   close(fd_2);
}

CgiHandler::~CgiHandler(void){}

CgiHandler::CgiHandler(const char *script, char * const *env, const char *body)
{
  
   if (initialize_pipe() < 0)
      return ;
   pid = fork();
   switch(pid) {
   case -1:
      _response_num = 500;
      _err = strerror(errno);
      break ;
   case 0:
      child_exe(script, env);
      break ;
   default:
      parent_exe(body);
   }
}

void CgiHandler::parent_exe(const char *body)
{
   signal(SIGTERM, st_sig_kill);
   st_close_fd(_fd_in[0], _fd_out[1]);
   close(_fd_err[1]);
   write(_fd_in[0], body, strlen(body));
   close(_fd_in[0]);
   char buffer[100];
   while (read(_fd_out[0], buffer, 100) > 0)
   {
      _body.append(buffer);
      memset(buffer, '\0', 100);
   }
   close (_fd_out[0]);
   while (read(_fd_err[0], buffer, 100) > 0)
   {
      _body_err.append(buffer);
      memset(buffer, '\0', 100);
   }
   close (_fd_err[0]);
   int *status = NULL;
   waitpid(pid, status);
   if (status)
   {
      if (*status == 137)
         _response_num = 137;
      else
      {  
         _response_num = 500;
         _err = strerror(errno);
      }
   }
}

void CgiHandler::child_exe(const char *script, char * const *env)
{
   close(_fd_in[1]);
   close(_fd_out[0]);
   close(_fd_err[0]);
   char* argv[]={(char*)script, (char*)script,  NULL};
   if (dup2(_fd_in[0], STDIN_FILENO) < 0)
   {
      close(_fd_in[0]);
      st_close_fd(_fd_out[1], _fd_err[1]);
      exit(errno);
   }
   close(_fd_in[0]);
   if (dup2(_fd_out[1], STDOUT_FILENO) < 0)
   {
      close(STDIN_FILENO);
      st_close_fd(_fd_out[1], _fd_err[1]);
      exit(errno);
   }
   close(_fd_out[1]);
   if (dup2(_fd_err[1], STDERR_FILENO) < 0)
   {
      close(STDIN_FILENO);
      st_close_fd(STDOUT_FILENO, _fd_err[1]);
      exit(errno);
   }
   close(_fd_err[1]);
   
   execve(script, argv, env);
   exit (errno);
}


int CgiHandler::initialize_pipe (void)
{
   if (pipe(_fd_in) < 0)
   {
      _response_num = 500;
      _err = "Pipe failed";
      return -1;
   }
   if( pipe(_fd_out) < 0)
   {
      st_close_fd(_fd_in[0], _fd_in[1]);
       _response_num = 500;
      _err = "Pipe failed";
      return -1;
   }
   if( pipe(_fd_err) < 0)
   {
      st_close_fd(_fd_in[0], _fd_in[1]);
      st_close_fd(_fd_out[0], _fd_out[1]);
       _response_num = 500;
      _err = "Pipe failed";
      return -1;
   }
      return 0;
}

int CgiHandler::get_response_num() const
{
   return _response_num;
} 

std::string CgiHandler::get_response() const
{
   return _body;
}
