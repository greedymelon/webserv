#include "CgiHandler.hpp"
#include <cstring>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/event.h>
#include <unistd.h>
#include <fcntl.h>
#include <string>
#include <signal.h>
#include <time.h>
#include <chrono>
#include <iostream>
#define MAX_TIME 20000

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
	
	int pid = fork();
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
	struct kevent event[2];
	EV_SET(&event[0], _fd_in[1], EVFILT_WRITE, EV_ADD, 0, 0, NULL);
	EV_SET(&event[1], _fd_out[0], EVFILT_READ, EV_ADD, 0, 0, NULL);


	int kque = kqueue();
	kevent(kque, event, 2, NULL, 0, NULL);
	auto start = std::chrono::high_resolution_clock::now();
	st_close_fd(_fd_in[0], _fd_out[1]);
	int read_finished = 0;
	int write_finished = 0;
	char buffer[200];
	memset(buffer, '\0', 100);
	kevent(kque, NULL, 0, event, 2, NULL);
	while (1)
	{
		for (int i = 0; i < 2; i++) 
		{
			if (event[i].filter == EVFILT_READ)
			{
				if (read(_fd_out[0], buffer, 200) < 1)
					read_finished++;
				_response.append(buffer);
			}
			if(event[i].filter == EVFILT_WRITE)
			{
				write(_fd_in[1], body, strlen(body));
				write_finished++;
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
		if (read_finished && write_finished)
			break ;
	}

	close(kque);
	if (_fd_in[1])
		close(_fd_in[1]);
	close (_fd_out[0]);

	int status;
	wait(&status);
	if (status)
	{
		if (status == 137)
		{
			_response_num = 137;
			_err = "child was killed";
		}
		else
		{
			_response_num = 500;
			_err = strerror(status);
		}
	}
	else
		_response_num = 200;
}

void CgiHandler::child_exe(const char *script, char * const *env)
{
	close(_fd_in[1]);
	close(_fd_out[0]);
	char* argv[]={(char*)script, (char*)script,  NULL};
	
	if (dup2(_fd_in[0], STDIN_FILENO) < 0)
	{
		st_close_fd(_fd_in[0], _fd_out[1]);
		exit(errno);
	}
	close(_fd_in[0]);
	if (dup2(_fd_out[1], STDOUT_FILENO) < 0)
	{
		st_close_fd(STDIN_FILENO, _fd_out[1]);
		exit(errno);
	}
	close(_fd_out[1]);

	execve(script, argv, env);
	int error = errno;
	st_close_fd(STDIN_FILENO, STDOUT_FILENO);
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
	return 0;
}

std::string  CgiHandler::get_error (void) const
{
	return _err;
}
int CgiHandler::get_response_num() const
{
	return _response_num;
} 

std::string CgiHandler::get_response() const
{
	return _response;
}
