#include "Server.hpp"

Server::Server(std::string port, std::string pw) : _port(port), _nconections(0)
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
	// for (unsigned long i = 0; i < _pfds.size(); i++)
	// 	close(_pfds[i].fd);
	// _pfds.clear();

	if (_sockfd)
		close(_sockfd);
	std::cout << "destructor called" << std::endl;
}

void Server::start()
{
	try
	{
		// create a socket on machine
		this->createSocket();

		// bind that socket
		this->bindSocket();

		// listen on that socket
		this->listenSocket();

		this->startPoll();

		// while(1) {  // main accept() loop
		// 	try
		// 	{
		// 		new_fd = this->acceptConnection();
		// 		this->sendData(new_fd, std::string("Welcome! Are you there?\n"));
		// 	}
		// 	catch(const std::exception& e)
		// 	{
		// 		std::cerr << e.what() << '\n';
		// 	}
		// 	// if (new_fd != -1)
		// 	// 	close(new_fd);
		// }
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << '\n';
	}
}

void Server::createSocket()
{
	int	yes=1;

	this->_sockfd = socket(_servinfo->ai_family, _servinfo->ai_socktype, _servinfo->ai_protocol);
	if (_sockfd == -1)
		throw std::runtime_error("socket: failed");
	if (setsockopt(_sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1)
		throw std::runtime_error("socketopt");
}

void Server::bindSocket()
{
	struct addrinfo		*p;

	for (p = _servinfo; p != NULL; p = p->ai_next)
	{
		try
		{
			if (bind(_sockfd, p->ai_addr, p->ai_addrlen) == -1)
				throw std::runtime_error("server: bind failed");
			break ;
		}
		catch(const std::exception& e)
		{
			close(_sockfd);
			std::cerr << e.what() << '\n';
		}
	}
	if (!p)
		throw std::runtime_error("server: failed to bind");
}

void Server::listenSocket()
{
	freeaddrinfo(_servinfo);
	if (listen(_sockfd, 10) == -1)
		throw std::runtime_error("server: listen error");
	
	struct pollfd listener;

	listener.fd = _sockfd;
	listener.events = POLLIN; // report ready to read on incoming session
	_pfds.push_back(listener);
	log("server is listening...");
}



/*  This function only works for ipv4 -- NEED TO REVIEW */
int Server::acceptConnection( void )
{
	int							new_fd;
	socklen_t					sin_size;
	struct sockaddr_storage		their_addr; // connector's address information
	
	sin_size = sizeof(their_addr);
	new_fd = accept(_sockfd, reinterpret_cast<sockaddr *>(&their_addr), &sin_size);
	if (new_fd == -1)
		throw std::runtime_error("accept failed");
	log(std::string("got connection from ").append(inet_ntoa((reinterpret_cast<sockaddr_in *>(&their_addr))->sin_addr)));
	_nconections++;
	return (new_fd);
}

void Server::startPoll( void )
{
	struct pollfd	temp;
	char buf[256];    // Buffer for client data

	while(1)
	{
        int poll_count = poll(&_pfds[0], _pfds.size(), -1);

        if (poll_count == -1)
		{
            // log("poll");
			break ;
		}

        // Run through the existing connections looking for data to read
        for(unsigned long i = 0; i < _pfds.size(); i++) {

            // Check if someone's ready to read
            if (_pfds[i].revents & POLLIN)
			{
				// We got one!!
                if (_pfds[i].fd == _sockfd) // If listener is ready to read, handle new connection
				{
					temp.fd = this->acceptConnection();
					temp.events = POLLIN;
                    _pfds.push_back(temp);
                }
				else
				{
                    // just a regular client
                    int nbytes = recv(_pfds[i].fd, buf, sizeof(buf), 0);
                    int sender_fd = _pfds[i].fd;

                    if (nbytes <= 0) // Got error or connection closed by client
					{
                        if (nbytes == 0)
                            log("client disconnected");
						else
                            log("recv");
                        close(_pfds[i].fd); // Bye!
                        _pfds.erase(_pfds.begin() + i);
                    }
					else
					{
                        // We got some good data from a client
                        for(unsigned long j = 0; j < _pfds.size(); j++)
						{
                            // Send to everyone!
                            int dest_fd = _pfds[j].fd;

                            // Except the listener and ourselves
                            if (dest_fd != _sockfd && dest_fd != sender_fd) {
                                if (send(dest_fd, buf, nbytes, 0) == -1) {
                                    perror("send");
                                }
                            }
                        }
                    }
                } // END handle data from client
            } // END got ready-to-read from poll()
        } // END looping through file descriptors
    } // END for(;;)--and you thought it would never end!
}



void Server::sendData( int sockfd , const std::string &msg)
{
	if (send(sockfd, msg.c_str(), msg.length(), 0) == -1)
		throw std::runtime_error("send from server failed");
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
