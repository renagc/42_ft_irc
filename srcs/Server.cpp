#include "Server.hpp"

Server::Server(std::string port, std::string pw) : _port(port)
{
	int portv = std::atoi(port.c_str());
	if (portv <= PORT_MIN_VALUE || portv >= PORT_MAX_VALUE)
		throw ServerPortException();
	if (pw.length() == 0)
		throw ServerWeakPWException();
	
	struct addrinfo		hints;
	int					status;

	std::memset(&hints, 0, sizeof(hints)); // Initialize hints to all zeroes
	hints.ai_flags = AI_PASSIVE; // Fill my ip
	hints.ai_family = AF_UNSPEC; // Allow IPv4 or IPv6
    hints.ai_socktype = SOCK_STREAM; // Use TCP socket

	status = getaddrinfo(NULL, (this->_port).c_str(), &hints, &(this->_servinfo));
	if (status != 0)
		throw std::runtime_error(gai_strerror(status));
}

Server::~Server()
{
	freeaddrinfo(_servinfo);
}

struct addrinfo *Server::getAddrInfo()
{
	return(this->_servinfo);
}

void Server::socketBind()
{
	struct addrinfo		*p;
	int					yes=1;

	for (p = _servinfo; p != NULL; p = p->ai_next)
	{
		this->_sockfd = socket(_servinfo->ai_family, _servinfo->ai_socktype, _servinfo->ai_protocol);
		if (_sockfd == -1)
		{
			log("socket: failed");
			continue;
		}
		if (setsockopt(_sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1)
			throw std::runtime_error("socketopt");
		if (bind(_sockfd, p->ai_addr, p->ai_addrlen) == -1)
		{
			close(_sockfd);
			log("bind: failed");
			continue;
		}
		break ;
	}

	if (!p)
		throw std::runtime_error("server: failed to bind");
}

void Server::listener()
{
	// struct sigaction sa;

	if (listen(_sockfd, 10) == -1)
		throw std::runtime_error("server: listen error");
	
	// sa.sa_restorer = sigchld_handler;
	std::cout << "Server is listening..." << std::endl;

}

void Server::log( std::string str )
{
	time_t rawtime;
	struct tm *timeinfo;

	time(&rawtime);
	timeinfo = localtime(&rawtime);
	std::cout << timeinfo->tm_year + 1900 << "-" << \
				timeinfo->tm_mon + 1 << "-" << \
				timeinfo->tm_mday << " " << \
				timeinfo->tm_zone << " " << \
				timeinfo->tm_hour << ":" << \
				timeinfo->tm_min << ":" << \
				timeinfo->tm_sec << " | " << \
				"server: " << str << std::endl;
}
