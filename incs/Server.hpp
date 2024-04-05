#ifndef SOCKET_HPP
# define SOCKET_HPP

# include <iostream>
# include <cstdlib>
#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <cstdio>
#include <csignal>
#include <signal.h>
#include <cstring>

# define PORT_MIN_VALUE 1023
# define PORT_MAX_VALUE 65535

class Server
{
	private:
		std::string			_port;
		std::string			_password;
		struct addrinfo 	*_servinfo;
		int					_sockfd;

	public:
		Server(std::string port, std::string pw);
		~Server();

		/* member functions */
		struct addrinfo		*getAddrInfo();
		void				socketBind();
		void				listener();
		void				log( std::string str );

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