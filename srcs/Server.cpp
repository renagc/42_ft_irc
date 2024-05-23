#include "../incs/Server.hpp"

Server::Server(std::string port, std::string pw) : _port(port), _next_client_id(0), _next_channel_id(0)
{
	int portv = std::atoi(port.c_str());
	if (portv <= PORT_MIN_VALUE || portv >= PORT_MAX_VALUE)
		throw std::runtime_error("server port error");
	if (pw.length() < 2)
		throw std::runtime_error("server weak password");

	//set valid commands;
	_commands.push_back("JOIN");
	_commands.push_back("NICK");
	_commands.push_back("USER");

	struct addrinfo		hints;
	int					status;

	// Address configuration
	std::memset(&hints, 0, sizeof(hints)); // Initialize hints to all zeroes
	hints.ai_flags = AI_PASSIVE; // Fill my ip
	hints.ai_family = AF_UNSPEC; // Allow IPv4 or IPv6
    hints.ai_socktype = SOCK_STREAM; // Use TCP socket

	status = getaddrinfo(NULL, (this->_port).c_str(), &hints, &(this->_servinfo));
	if (status != 0)
		throw std::runtime_error(gai_strerror(status));
	
	// Create a socket
	int		yes=1;

	_sockfd = socket(_servinfo->ai_family, _servinfo->ai_socktype, _servinfo->ai_protocol);
	if (_sockfd == -1)
		throw std::runtime_error("socket: failed");
	if (setsockopt(_sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1)
		throw std::runtime_error("socketopt");

	// Bind the socket
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

Server::~Server()
{
	for (unsigned long i = 0; i < _pfds.size(); i++)
		close(_pfds[i].fd);
	_pfds.clear();
	close(_sockfd);
	std::cout << "destructor called" << std::endl;
}

void Server::startListen( void )
{
	freeaddrinfo(_servinfo);
	
	try {
		if (listen(_sockfd, 10) == -1)
			throw std::runtime_error("server: listen error");
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << '\n';
	}
	
	struct pollfd listener;

	listener.fd = _sockfd;
	listener.events = POLLIN; // report ready to read on incoming session
	_pfds.push_back(listener);
	LOG(std::string("server is listening on localhost:").append(_port));

	// Poll loop for server to listen for incoming data from/to clients
	while(1)
	{
        if (poll(&_pfds[0], _pfds.size(), -1) == -1)
			throw std::runtime_error("poll failed");

        // Run through the existing connections
        for(unsigned long i = 0; i < _pfds.size(); i++) {
            if (_pfds[i].revents && POLLIN)
			{
                if (_pfds[i].fd == _sockfd) // If listener is ready to read, handle new connection
					this->clientConnection();
				else
					this->knownConnection( i );
            }
        }
    }
}

void Server::clientConnection( void )
{
	pollfd						temp;
	socklen_t					sin_size;
	struct sockaddr_storage		their_addr; // connector's address information
	
	sin_size = sizeof(their_addr);
	temp.fd = accept(_sockfd, reinterpret_cast<sockaddr *>(&their_addr), &sin_size);
	if (temp.fd == -1)
		throw std::runtime_error("accept failed");
	LOG(std::string("got connection from ").append(inet_ntoa((reinterpret_cast<sockaddr_in *>(&their_addr))->sin_addr)));
	temp.events = POLLIN;
	_pfds.push_back(temp);

	std::map<int, Client>::iterator	it = _clients.find(temp.fd);
	if (it != _clients.end())
		return ;
	Client	new_client( temp.fd, _next_client_id++, std::string(inet_ntoa((reinterpret_cast<sockaddr_in *>(&their_addr))->sin_addr)));
	_clients.insert(std::pair<int,Client>(temp.fd,new_client));
	log(std::string("client logging in"), new_client.getId());
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
	log("channel created", newchannel.getId());
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

void Server::authenticateChecker( Client *client )
{
	if (client->getNick().empty() || client->getUser().empty())
		return ;
	client->setLogged(true);
	throw RPL_WELCOME(client->getNick(), client->getUser());
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
			log("client disconnected", client->getId());
		else
			log("recv");
		clientDisconnect(client);
	}
	else
	{
		msg.assign(buf, nbytes);
		std::cout << msg;
		std::vector<std::string> parse = split(msg, "\n");
		std::vector<std::string>::iterator it;
		for (it = parse.begin(); it != parse.end(); it++)
		{
			std::vector<std::string> cmd = split(*it, " ");
			findCommand(client, cmd);
		}
		handleDataSender(msg, client);
	}
}

// command chooser
void Server::chooseCommand( Client *client, const std::vector<std::string> &cmd )
{
	try
	{
		if (!cmd[0].compare("NICK"))
			nickCommand(client, cmd[1]);
		else if (!cmd[0].compare("USER"))
			userCommand(client, cmd[1]);
		else if (!cmd[0].compare("JOIN"))
			joinCommand(client, cmd[1]);
	}
	catch(const std::string& e)
	{
		if (send(client->getFd(), e.c_str(), e.size(), 0) == -1)
			log("send problem");
	}
}

void Server::findCommand( Client *client, const std::vector<std::string> &cmd )
{
	std::vector<std::string>::iterator it = _commands.begin();
	for (; it != _commands.end(); it++)
	{
		if (!cmd[0].compare(*it))
			break ;
	}
	if (it != _commands.end())
		chooseCommand(client, cmd);
}

// commands functions

Client *Server::getClient( const std::string &nickname )
{
	std::map<int, Client>::iterator it;
	for (it = _clients.begin(); it != _clients.end(); it++)
	{
		if (it->second.getNick() == nickname)
			break ;
	}
	if (it == _clients.end())
		return (NULL);
	return (&it->second);
}

// JOIN #channel need to implement all the error messages
void Server::joinCommand( Client *client, const std::string &channel_name )
{
	DEBUG("joinCommand");
	if (channel_name.empty() || channel_name.at(0) != '#')
	{
		log("no channel name given", client->getId());
		throw ERR_NEEDMOREPARAMS(client->getNick(), "JOIN");
	}
	Channel	*channel = NULL;
	std::map<std::string, Channel>::iterator it = _channels.find(&channel_name[1]);
	if (it != _channels.end())
		channel = &it->second;
	if (!channel)
	{
		createChannel(&channel_name[1], client);
		channel = &_channels.find(&channel_name[1])->second;
	}
	if (client->getLogged())
	{
		channel->add(client);
		log("client joined channel", client->getNick(), channel->getName());
		throw RPL_JOIN(client->getNick(), client->getUser(), channel->getName());
	}
}

std::map<int, Client>::iterator Server::findNick( const std::string &nick )
{
	std::map<int, Client>::iterator it;
	for (it = _clients.begin(); it != _clients.end(); it++)
	{
		if (it->second.getNick() == nick)
			break ;
	}
	return (it);
}

// atencao ao parsing que esta a dar erro
void Server::nickCommand( Client *client, const std::string &nickname )
{
	
	if (nickname.empty())
	{
		log(std::string("no nickname given"));
		throw ERR_NONICKNAMEGIVEN(client->getNick());
	}
	std::map<int, Client>::iterator it = findNick(nickname);
	if (it != _clients.end())
	{
		log(std::string("error nickname in use"));
		throw ERR_NICKNAMEINUSE(client->getNick(), nickname);
	}
	client->setNick(nickname);
	log(std::string("client changed nickname to: ").append(nickname), client->getId());
	if (!client->getLogged())
		authenticateChecker(client);
}

void Server::userCommand( Client *client, const std::string &username )
{
	if (username.empty())
	{
		log(std::string("no username or realname given"));
		throw ERR_NEEDMOREPARAMS(client->getNick(), "USER");
	}
	client->setUser(username);
	log(std::string("client changed username to: ").append(username), client->getId());
	if (!client->getLogged())
		authenticateChecker(client);
}

void Server::debug( void )
{
	log("debugging users:");
	for (std::map<int, Client>::iterator it = _clients.begin(); it != _clients.end(); it++)
		it->second.printPrivate();

	for (std::map<std::string, Channel>::iterator it = _channels.begin(); it != _channels.end(); it++)
		it->second.printPrivate();
}
