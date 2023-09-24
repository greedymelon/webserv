#include "../inc/Server.hpp"

Server::Server(Config &conf) : _conf(conf) {}

Server::~Server() {}

void Server::setup()
{
	_kqueue = kqueue();
	
	if (_kqueue == -1) {
		std::perror("ERROR : kqueue()");
		exit(EXIT_FAILURE);
	}
	for (size_t i = 0; i < _conf.servers.size(); i++) { 
		Socket socket(_conf.servers[i].port);

		try {
			socket.SocketBindListen();
			UpdateKqueue(socket.getfd(), EVFILT_READ, EV_ADD, 0);
			_listeningSockets.insert(std::make_pair(socket.getfd(), socket));
			printLog(socket, "listening...");
		}
		catch (Socket::SocketCreateError& e) {
			std::perror(e.what());
			continue;
		}
		catch (std::runtime_error& e) {
			std::perror(e.what());
			close(socket.getfd());
			continue;
		}
	}
}

void Server::UpdateKqueue(int fd, int filter, int flag, int data) 
{
	EV_SET(&_changeList, fd, filter, flag, 0, data, NULL);
	if (kevent(_kqueue, &_changeList, 1, NULL, 0, NULL) == -1)
		throw std::runtime_error("[ERROR] updateKqueue() failed");
}

void Server::start()
{
	struct timespec timeout = { 0, 0 };

	setup();
	
	/* Event loop that monitors the kqueue for events. 
	Depending on the type of event, it calls different functions*/
	while (!_listeningSockets.empty())
	{
		try {
			int new_event = kevent(_kqueue, NULL, 0, &_eventList, 1, &timeout);
			if (new_event == 0)									// Continue if no new events
				continue;
			if (new_event == -1 || _eventList.flags & EV_ERROR)
				throw KeventError();
			else if (_eventList.flags & EV_EOF ||				// Close connection on EOF or timer event
					_eventList.filter == EVFILT_TIMER)
				closeConnection(_eventList);
			else if (isSockListening(_eventList))				// Handle new client connections
				acceptClient(_eventList);
			else if (_eventList.filter == EVFILT_READ)			// Handle read events
				receiveRead(_eventList);
			else if (_eventList.filter == EVFILT_WRITE)			// Handle write events
				sendWrite(_eventList);
		}
		catch (Socket::AcceptError& e) {
			std::perror(e.what());
		}
		catch (const std::runtime_error& e) {
			std::perror(e.what());
			closeConnection(_eventList);
		}
		catch (KeventError& e) {
			std::perror(e.what());
			close(_kqueue);
			closeAllConnections();
			closeListeningSockets();
			start();
		}
	}
}

/* Checks if the socket descriptor (event.ident) is found in the _listeningSockets map*/
bool Server::isSockListening(struct kevent& event) 
{
	std::map<int, Socket>::iterator it = _listeningSockets.find(event.ident);
	return (it != _listeningSockets.end()) ? true : false;
}

/* Accepts new client connections and update event monitoring*/
void Server::acceptClient(struct kevent& event) 
{	
	for (int i = 0; i < event.data; i++) {
		int connectionSocket = _listeningSockets[event.ident].socketAccept(); // Accept a new client connection
		printLog(_listeningSockets[event.ident], "Connecting...", connectionSocket);
		UpdateKqueue(connectionSocket, EVFILT_READ, EV_ADD, 0); // Set a read event filter for the connectionSocket and add it to the kqueue changeList
		struct SocketData data;									// Create a SocketData struct to hold connection information
		data.port = _listeningSockets[event.ident].getPort();
		_Clients.insert(std::make_pair(connectionSocket, data)); // Store the connectionSocket and its data in the Clients map
	}
}


void Server::receiveRead(struct kevent& event) 
{
	UpdateKqueue(event.ident, EVFILT_TIMER, EV_ADD | EV_ONESHOT, 5 * 1000); //Read ends in 5 seconds
	char buffer[SIZE];
	int num_bytes = recv(event.ident, buffer, sizeof(buffer) - 1, 0);
	if (num_bytes <= 0)
		throw std::runtime_error("ERROR : recv()");
	buffer[num_bytes] = '\0';
	printLog(event, YELLOW, "Receiving...", buffer);

}

void Server::sendWrite(struct kevent& event) 
{
	UpdateKqueue(event.ident, EVFILT_TIMER, EV_ADD | EV_ONESHOT, 60 * 1000);

}

void Server::closeConnection(struct kevent& event) {

	if (event.filter == EVFILT_TIMER)
		printLog(event, PURPLE, "Timeout...");
	else
		printLog(event, "", "Disconnecting...");
	close(event.ident);
	_Clients.erase(event.ident);
}

void Server::closeAllConnections() {

	std::map<int, struct SocketData>::iterator it;
	for (it = _Clients.begin(); it != _Clients.end(); ++it)
		close(it->first);
	_Clients.clear();
}

void Server::closeListeningSockets() {

	std::map<int, Socket>::iterator it;
	for (it = _listeningSockets.begin(); it != _listeningSockets.end(); ++it)
		close(it->first);
	_listeningSockets.clear();
}

void Server::printLog(Socket socket, std::string activity)
{
	std::cout << RED << getTime() << socket << std::setw(17) << activity;
	std::cout << GREEN << "\t" << socket.getfd() << RESET << std::endl;
}

void Server::printLog(Socket socket, std::string activity, int fd)
{
	std::cout << RED << getTime() << socket << std::setw(17) << activity;
	std::cout << GREEN << "\t" << socket.getfd() << " >> " << fd << RESET << std::endl;
}

void Server::printLog(struct kevent& event, std::string color, std::string activity) 
{
	std::cout << RED << getTime() << event << color << std::setw(17) << activity;
	std::cout << GREEN << "\t" << event.ident << RESET << std::endl << RESET;;
}
void Server::printLog(struct kevent& event, std::string color, std::string activity, std::string httpMessage) 
{
	std::cout << RED << getTime() << event << std::setw(17);
	std::cout << activity << GREEN << std::setw(8) << event.ident;
	std::cout << std::endl << std::endl << color << httpMessage.substr(0, 500);
	std::cout  << RESET << std::endl << std::endl;
}

std::ostream& operator<<(std::ostream &os, struct kevent& event) {

	struct sockaddr_in addr;
	socklen_t addrlen = sizeof(addr);
	getsockname(event.ident, (struct sockaddr *)&addr, &addrlen);
	os << BLUE << std::setw(8) << ntohs(addr.sin_port) << "\t" << RESET;
	return os;
}