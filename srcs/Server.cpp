#include "Server.hpp"

Server::Server(std::string port, std::string pw) : _port(port), _next_client_id(0), _next_channel_id(0)
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
	for (unsigned long i = 0; i < _pfds.size(); i++)
		close(_pfds[i].fd);
	_pfds.clear();

	if (_sockfd)
		close(_sockfd);
	std::cout << "destructor called" << std::endl;
}

void Server::start()
{
	try
	{
		_sockfd = this->createSocket();
		this->bindSocket(_sockfd);
		this->listenSocket();
		this->startPoll();
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << '\n';
	}
}

int Server::createSocket()
{
	int		sockfd;
	int		yes=1;

	sockfd = socket(_servinfo->ai_family, _servinfo->ai_socktype, _servinfo->ai_protocol);
	if (sockfd == -1)
		throw std::runtime_error("socket: failed");
	if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1)
		throw std::runtime_error("socketopt");
	return (sockfd);
}

void Server::bindSocket( int sockfd )
{
	struct addrinfo		*p;

	for (p = _servinfo; p != NULL; p = p->ai_next)
	{
		try
		{
			if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1)
				throw std::runtime_error("server: bind failed");
			break ;
		}
		catch(const std::exception& e)
		{
			close(sockfd);
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
	log(std::string("server is listening on localhost:").append(_port));
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
	return (new_fd);
}

void Server::startPoll( void )
{
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
					this->clientConnection();
				else
					this->knownConnection( i );
            } // END got ready-to-read from poll()
        } // END looping through file descriptors
    } // END for(;;)--and you thought it would never end!
}

void Server::clientConnection( void )
{
	pollfd	temp;

	temp.fd = this->acceptConnection();
	temp.events = POLLIN;
	_pfds.push_back(temp);

	std::map<int, Client>::iterator	it = _clients.find(temp.fd);
	if (it != _clients.end())
		return ;
	Client	new_client( temp.fd, _next_client_id++ );
	_clients.insert(std::pair<int,Client>(temp.fd,new_client));
	log("new client added", new_client.getId(), new_client.getNick());
	send(temp.fd, "welcome to ft_irc!\n", 19, 0);
}

Client *Server::findClientByFd( int fd )
{
	Client *client = NULL;
	std::map<int, Client>::iterator it_client = _clients.find(fd);
	if (it_client != _clients.end())
		client = &it_client->second;
	return (client);
}


// this function removes a client from the server and their channels
void Server::clientDisconnect( Client *client )
{
	// loop channels to remove client from all channels
	for (std::map<std::string, Channel>::iterator it = _channels.begin(); it != _channels.end(); it++)
		it->second.removeClient(client);

	// close client fd
	close(client->getFd());

	// loop clients to remove the client
	std::map<int, Client>::iterator it;
	for (it = _clients.begin(); it != _clients.end(); it++)
	{
		if (&it->second == client)
			break ;
	}

	// loop pfds to stop from poll
	unsigned long i = 0;
	for (; i < _pfds.size(); i++)
	{
		if(_pfds[i].fd == client->getFd())
			break ;
	}

	_pfds.erase(_pfds.begin() + i);
	_clients.erase(it);
}

void Server::createChannel( const std::string &name, Client *admin )
{
	Channel	newchannel("test", admin);
	newchannel.setId(_next_channel_id++);
	_channels.insert(std::pair<std::string, Channel>(name, newchannel));
	log("channel created", newchannel.getId(), newchannel.getName());
}


void Server::handleDataSender( const std::string &msg, Client *sender )
{
	// We got some good data from a client
	for (std::map<std::string, Channel>::iterator it = _channels.begin(); it != _channels.end(); it++)
	{
		if (!it->second.findClient(sender))
			continue ;
		for(unsigned long j = 1; j < _pfds.size(); j++)
		{
			// Send to everyone!
			int dest_fd = _pfds[j].fd;
			Client *client = &_clients.find(dest_fd)->second;

			// Except the listener and ourselves
			if (	dest_fd != _sockfd && \
					dest_fd != sender->getFd() && \
					it->second.findClient(client))
			{
				if (send(dest_fd, msg.c_str(), msg.size(), 0) == -1)
					perror("send");
				else
					log("data sent", sender->getNick(), client->getNick());
			}
		}
		debug();
	}
}


void Server::knownConnection( int id )
{
	char			buf[256];
	int				sender_fd = _pfds[id].fd;
	size_t			nbytes = recv(sender_fd, buf, sizeof(buf), 0);
	Client			*client = findClientByFd(sender_fd);

	std::string		msg;
	if (nbytes <= 0) // Got error or connection closed by client
	{
		if (nbytes == 0)
			log("client disconnected", client->getId(), client->getNick());
		else
			log("recv");
		clientDisconnect(client);
	}
	else
	{
		msg.assign(buf, nbytes);
		if (msg.compare("JOIN\n") == 0)
		{
			std::map<int, Client>::iterator it_client = _clients.find(sender_fd);
			std::map<std::string, Channel>::iterator it_channel = _channels.find("test");

			Channel *channel = NULL;
			Client *client = NULL;
			if (it_client != _clients.end())
				client = &it_client->second;
			if (it_channel != _channels.end())
				channel = &it_channel->second;
			if (channel)
			{
				if (channel->getAdmin() == client) // client or sender is the admin
					return;
				std::vector<Client *> temp = channel->getClients();
				for (unsigned long i = 0; i < temp.size(); i++)
				{
					if (client == temp[i]) // client or sender is already in channel
					{
						return ;
					}
				}
				channel->add(client);
				return ;
			}
			createChannel("test", client);
			return ;
		}
		else if (msg.compare("PARA\n") == 0)
		{
			std::map<int, Client>::iterator it_client = _clients.find(sender_fd);
			std::map<std::string, Channel>::iterator it_channel = _channels.find("para");

			Channel *channel = NULL;
			Client *client = NULL;
			if (it_client != _clients.end())
				client = &it_client->second;
			if (it_channel != _channels.end())
				channel = &it_channel->second;
			if (channel)
			{
				if (channel->getAdmin() == client) // client or sender is the admin
				{
					return;
				}
				std::vector<Client *> temp = channel->getClients();
				for (unsigned long i = 0; i < temp.size(); i++)
				{
					if (client == temp[i]) // client or sender is already in channel
					{
						return ;
					}
				}
				channel->add(client);
				return ;
			}
			createChannel("para", client);
			return ;
		}
		handleDataSender(msg, client);
	}
}

void Server::sendData( int sockfd , const std::string &msg)
{
	if (send(sockfd, msg.c_str(), msg.length(), 0) == -1)
		throw std::runtime_error("send from server failed");
}


void Server::log( const std::string &str )
{
	printLocalTime();
	std::cout << " | " << "server: " << str << std::endl;
}

void Server::log( const std::string &str, const std::string &from, const std::string &to )
{
	printLocalTime();
	std::cout << " | " << "server: " << str << " from: " << from << ", to: " << to <<std::endl;
}

void Server::log( const std::string &str, int id, const std::string &name )
{
	printLocalTime();
	std::cout << " | " << "server: " << str << " | id: " << id << ", name: " << name << std::endl;
}

void Server::printLocalTime( void )
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
				timeinfo->tm_sec;
}

void Server::debug( void )
{
	log("debugging users:");
	for (std::map<int, Client>::iterator it = _clients.begin(); it != _clients.end(); it++)
		it->second.printPrivate();

	for (std::map<std::string, Channel>::iterator it = _channels.begin(); it != _channels.end(); it++)
		it->second.printPrivate();
}
