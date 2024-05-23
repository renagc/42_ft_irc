#include "Parser.hpp"

// socket library
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>

#include <cstring>
#include <cstdio>

#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>

/*
	int getaddrinfo(const char *node, // e.g. "www.example.com" or IP
					const char *service, // e.g. "http" or port number
					const struct addrinfo *hints,
					struct addrinfo **res);
*/

#include "Server.hpp"

// bool server_status = true;

// void sigIntHandler( int wnum )
// {
// 	(void)wnum;
// 	server_status = false;
// }

int main(int ac, char **av)
{
	try
	{
		if (ac != 3)
			throw std::runtime_error("Usage: ./ircserv <port> <password>");
		Server serv(av[1], av[2]);
		serv.startListen();
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << '\n';
	}	
}