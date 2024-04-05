/*
** showip.c -- show IP addresses for a host given on the command line
*/

#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <cstring>


int main(int ac, char *av[])
{
	(void)ac;
	(void)av;
	struct addrinfo hints, *servinfo;

	std::memset(&hints, 0, sizeof hints); // Initialize hints to all zeroes
	hints.ai_flags = AI_PASSIVE; // Fill my ip
	hints.ai_family = AF_UNSPEC; // Allow IPv4 or IPv6
    hints.ai_socktype = SOCK_STREAM; // Use TCP socket
    hints.ai_protocol = IPPROTO_TCP; // Use TCP protocol


	// Getting servinfo
	int status = getaddrinfo("www.google.com", "http", &hints, &servinfo);
	if (status == 0)
		std::cout << "getaddrinfo success!" << std::endl;
	else
		std::cout << gai_strerror(status) << std::endl;

	// Make a socket
	int sockfd = socket(servinfo->ai_family, servinfo->ai_socktype, servinfo->ai_protocol);
	if (sockfd != -1)
		std::cout << "socket fd created! fd: " << sockfd << std::endl;
	else
		std::cout << "socket failed!" << std::endl;

	// Getting port nummber
	int port = bind(sockfd, servinfo->ai_addr, servinfo->ai_addrlen);
	if (port != -1)
		std::cout << "bind found! port: " << port << std::endl;
	else
		perror("ola");

	// Cleaning getaddrinfo servinfo memory
	freeaddrinfo(servinfo);
	return(0);
}
