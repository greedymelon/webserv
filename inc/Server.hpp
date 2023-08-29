#ifndef SERVER_HPP
# define SERVER_HPP

#include <unistd.h>
#include <stdlib.h>
#include <sys/event.h>
#include <iostream>
#include <vector>

#include "Socket.hpp"
#include "Configuration.hpp"

typedef struct s_socketData {

	bool				connected;
	struct sockaddr_in	*addr;
	std::string			host;
	std::string			port;

}						t_socketData;

// typedef struct s_socketData {

// 	int					flag;
// 	struct sockaddr_in	*addr;
// 	std::string			host;
// 	std::string			port;

// }						t_socketData;


class Server {

	private:

		Configuration	_conf;
		int				_kq;

		// void	accept_new_client( int kq, struct kevent &event );
		// void	read_socket( int kq, struct kevent &event );
		// void	write_socket( int kq, struct kevent &event );
		// int		is_listen_socket( struct kevent *kset, int fd, int len );
		// t_socketData	*init_udata( struct sockaddr_in *addr, std::string host, std::string port );
	

	public:

		Server( const Configuration &conf );
		~Server( void );
	
		void	start( void );
};

#endif