#ifndef SOCKET_HPP
# define SOCKET_HPP

# include <iostream>
# include <iomanip>
# include <cstdlib>
# include <iostream>
# include <sys/types.h>
# include <sys/socket.h>
# include <netdb.h>
# include <unistd.h>
# include <cstdio>
# include <csignal>
# include <csignal>
# include <cstring>
# include <netinet/in.h>
# include <arpa/inet.h>
# include <vector>
# include <map>
# include <poll.h>

#include <errno.h>

/*		IRC libs		*/
# include "Client.hpp"
# include "Channel.hpp"

# define PORT_MIN_VALUE 1023
# define PORT_MAX_VALUE 65535

/*
	Steps of a Server:
		initialization			getaddrinfo()
		create a socket			socket()
		bind a socket			bind()
		listen on the socket	listen()
		accept connection		accept()
		send and receive data	send(), recv(), recvfrom(), sento()
		disconnect				close()
*/

//		  uint32_t htonl(uint32_t hostlong);
//        uint16_t htons(uint16_t hostshort);

//        uint32_t ntohl(uint32_t netlong);
//        uint16_t ntohs(uint16_t netshort);

class Client;

class Server
{
	private:
		std::string					_port;
		struct addrinfo				*_servinfo;
		int							_sockfd;
		std::vector<pollfd>			_pfds;
		std::map<int, Client>		_clients;
		std::vector<Channel>		_channels;
		int							_next_id;


		/* debug */
		void						printLocalTime( void );
		void						log( std::string str );
		void						debug( void );


	public:
		Server(std::string port, std::string pw);
		~Server();

		/* member functions */

		// main function
		void				start( void );

		// steps to create a server
		int					createSocket( void );
		void				bindSocket( int sockfd );
		void				listenSocket( void );
		int					acceptConnection( void );

		// help functions
		void				startPoll( void );
		void				sendData( int sockfd, const std::string &info );

		// poll functions
		void				newConnection( void );
		void				knownConnection( int id );

		void				clientDisconnect( bool success, int fd );


		// find channel
		Channel		*findChannel( const std::string &name );
		bool		sameChannel( Client *src, Client *dst );

		/* exceptions */
		class ServerPortException : public std::exception
		{
			public:
				virtual const char *what() const throw() { return ("invalid port"); }
		};
		class ServerWeakPWException : public std::exception
		{
			public:
				virtual const char *what() const throw() { return ("invalid password"); }
		};
};

#endif