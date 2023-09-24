#ifndef SOCKET_HPP
# define SOCKET_HPP

# include <arpa/inet.h>
# include <fcntl.h>
# include <iomanip>
# include <iostream>
# include <netinet/in.h>
# include <sys/event.h>
# include <sys/socket.h>
# include <unistd.h>

# include "Config.hpp"
# include "Utils.hpp"

# define SIZE 65536		//64KB

class Socket {
private:
	int					_fd;
	std::string			_port;
	struct sockaddr_in	_addr;

public:
	Socket(std::string port);
	Socket();
	~Socket();

	void	SocketBindListen();
	void	setFdOptions(int fd);
	int		socketAccept();

	int			getfd();
	std::string getPort();
	sockaddr_in getAddr();

	class SocketCreateError : public std::exception {
		public:
			const char* what() const throw() {
				return "ERROR : Socket()";
			}
	};

	class AcceptError : public std::exception {
		public:
			const char* what() const throw() {
				return "ERROR : Accept()";
			}
	};

};

std::ostream& operator<<(std::ostream &os, Socket& obj);


#endif