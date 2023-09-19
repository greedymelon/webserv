#include "CgiHandler.hpp"
#include <cstring>
#include <sys/types.h>
#include <sys/wait.h>
//#include <sys/event.h>
#include <unistd.h>
#include <fcntl.h>
#include <string>
#include <signal.h>
#include <time.h>
#include <chrono>
#define MAX_TIME 20000


//pid_t pid;
// static void st_sig_kill(int signum)
// {
//    if (signum)
//       kill(pid, SIGKILL);
// }


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
  
	pid_t pid = fork();
	switch(pid) {
	case -1:
		_response_num = 500;
		_err = strerror(errno);
		break ;
	case 0:
		child_exe(script, env);
		break ;
	default:
		parent_exe(body, pid);
	}
}

void CgiHandler::parent_exe(const char *body, pid_t pid)
{
	//signal(SIGTERM, st_sig_kill);
	struct kevent event[3];
	struct timespec timeout = {0, 0};
	EV_SET(&event[0], _fd_in[1], EVFILT_WRITE, EV_ADD, 0, 0, NULL);
	EV_SET(&event[1], _fd_out[0], EVFILT_READ, EV_ADD, 0, 0, NULL);
	EV_SET(&event[2], _fd_err[0], EVFILT_READ, EV_ADD, 0, 0, NULL);
	int kque = kqueue();
	auto start = std::chrono::high_resolution_clock::now();
	st_close_fd(_fd_in[0], _fd_out[1]);
	close(_fd_err[1]);
	char buffer[100];
	int num_events = kevent(kque, NULL, 0, event, 3, &timeout);
	for (int i = 0; i < num_events; i++) 
	{
		if (event[i].filter == EVFILT_READ) 
		{
			if (read(event[i].ident, buffer, 100) < 1)
				close(event[i].ident)
			if (i == 1)
				_response.append(buffer);
			else
				_response_err.append(buffer);
			memset(buffer, '\0', 100);
		}
		if(event[i].filter == EVFILT_WRITE)
		{
			write(_fd_in[1], body, strlen(body));
			close (_fd_in[1]);
		}	
		auto corrent = std::chrono::high_resolution_clock::now();
		auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(corrent - start);
		if (duration.count() > MAX_TIME)
		{
			kill(pid, SIGKILL);
			break ;
		}
	}
	close(kque);
	//write(_fd_in[1], body, strlen(body));
	if (_fd_in[1] != -1)
		close(_fd_in[1]);
	//char buffer[100];
	// while (read(_fd_out[0], buffer, 100) > 0)
	// {
	//    _response.append(buffer);
	//    memset(buffer, '\0', 100);
	// }
	if (_fd_out[0] != -1)
		close (_fd_out[0]);
	// while (read(_fd_err[0], buffer, 100) > 0)
	// {
	//    _response_err.append(buffer);
	//    memset(buffer, '\0', 100);
	// }
	if (_fd_err[0] != -1)
		close (_fd_err[0]);
	int *status = NULL;
	wait(status);
	if (status)
	{
		if (*status == 137)
		{
			_response_num = 137;
			_err = "child was killed";
		}
		else
		{
			_response_num = 500;
			_err = strerror(*status);
		}
	}
	else
		_response_num = 200;
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
	int error = errno;
	close(STDIN_FILENO);
	st_close_fd(STDOUT_FILENO, STDERR_FILENO);
	exit (error);
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
	return _response;
}
