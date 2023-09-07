#include "../inc/Server.hpp"

void Server::start() {

	_kq = kqueue();
	if (_kq == -1) {
		std::perror("[ERROR] kqueue() failed");
		std::exit(EXIT_FAILURE);
	}
	//test
}