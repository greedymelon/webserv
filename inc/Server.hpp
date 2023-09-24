#ifndef SERVER_HPP
# define SERVER_HPP

#include <vector>

#include "./Socket.hpp"

struct SocketData {

	std::string	request;
	// std::string	response;
	std::string	port;
};

class Server {
private:
	Config			_conf;
	int				_kqueue;
	struct kevent	_changeList;
	struct kevent	_eventList;

	std::map<int, Socket>				_listeningSockets;
	std::map<int, struct SocketData>	_Clients;

	void UpdateKqueue(int fd, int filter, int flag, int data);
	bool isSockListening(struct kevent& event);
	void acceptClient(struct kevent& event);
	void receiveRead(struct kevent& event);
	void sendWrite(struct kevent& event);
	void closeConnection(struct kevent& event);
	void closeAllConnections();
	void closeListeningSockets();
	
	void printLog(Socket socket, std::string activity);
	void printLog(Socket socket, std::string activity, int fd);
	void printLog(struct kevent& event, std::string color, std::string activity);
	void printLog(struct kevent& event, std::string color, std::string activity, std::string httpMessage);

public:
	Server(Config &conf);
	~Server();

	void setup();
	void start();

	class KeventError : public std::exception {
		public:
			const char* what() const throw() {
				return "ERROR : kevent()";
			}
	};
	
};

std::ostream& operator<<(std::ostream &os, struct kevent& event);

#endif