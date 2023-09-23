#include "Socket.hpp"

Socket::Socket(std::string port) : _port(port) {}

Socket::Socket() {}

Socket::~Socket() {}

void	Socket::SocketBindListen()
{
	const int port = std::stoi(_port);

	if ((_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)	// Create a socket (IPv4, TCP)
		throw SocketCreateError();
	memset((char *)&_addr, 0, sizeof(_addr));			// Initialize the memory of the _addr structure to zeros
	_addr.sin_family = AF_INET;
	_addr.sin_addr.s_addr = htonl(INADDR_ANY);			// This allows the socket to accept connections on any available network interface
	_addr.sin_port = htons(port);						// htons converts a short integer (port) to a network byte order
	setFdOptions(_fd);
	if (bind(_fd, (struct sockaddr *) &_addr, sizeof(_addr)) < 0)	// bind the socket to a specific network address and port
		throw std::runtime_error("ERROR bind(): ");
	if (listen(_fd, SOMAXCONN) < 0)						//system-defined maximum length of pending connections that can be queued up before connections are refused.
		throw std::runtime_error("ERROR listen(): ");

}

void Socket::setFdOptions(int fd) 
{

	int optval = 1;
	if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) == -1)	//Allows another socket to immediately bind to the same address and port even if there is an existing socket bound to it.
		throw std::runtime_error("[ERROR] setsockopt(SO_REUSEADDR) failed");
	if (setsockopt(fd, SOL_SOCKET, SO_REUSEPORT, &optval, sizeof(optval)) == -1)	//Allows multiple sockets to bind to the same address and port, distributing the load.
		throw std::runtime_error("[ERROR] setsockopt(SO_REUSEPORT) failed");
	if (fcntl(fd, F_SETFL, O_NONBLOCK) == -1) 										//Set the socket to non-blocking mode,  socket operations won't block and will return immediately
		throw std::runtime_error("[ERROR] fcntl(O_NONBLOCK) failed");
}

/* accept() will return new socket fd to use for this single connection, suddenly you have two socket fd's.
The original one still istening for more new connections, and the this one is ready to send() and recv() */
int Socket::socketAccept() 
{

	int connectionSocket = accept(_fd, NULL, NULL);

	if (connectionSocket < 0)
		throw AcceptError();
	setFdOptions(connectionSocket);
	return connectionSocket;
}

int	Socket::getfd() {return _fd;}

std::string Socket::getPort() {return _port;}

sockaddr_in Socket::getAddr() {return _addr;}

/* Returns port number in outstream form */
std::ostream& operator<<(std::ostream &os, Socket& obj) 
{
	os << BLUE << std::setw(8) << ntohs(obj.getAddr().sin_port) << "\t" << RESET;
	return os;
}