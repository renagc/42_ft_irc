#include "../incs/Channel.hpp"

/* constructors */
Channel::Channel( const std::string &name, Client *admin ) : _name(name), _admin(admin)
{
	_clients.push_back(admin);
}

Channel::~Channel() {}

/* add */
void Channel::add( Client *client )
{ 
	_clients.push_back(client);
}

/* accessors */
const std::string &Channel::getName( void ) const { return(_name); }

const Client *Channel::getAdmin( void ) const { return(_admin); }

std::vector<std::string>	Channel::getNicknames( void ) const {
	std::vector<std::string> nicks;

	for(unsigned long i = 0; i < _clients.size(); i++)
		nicks.push_back(_clients[i]->getNick());

	return (nicks);
}

std::vector<Client *> Channel::getClients( void ) const { return(_clients); }
bool	Channel::findClient( Client *client ) const
{
	std::cout << _clients.size() << std::endl;
	for (unsigned long i = 0; i < _clients.size(); i++)
	{
		std::cout << _clients[i] << " vs " << client << std::endl;
		if (_clients[i] == client)
		{
			std::cout << " found" << std::endl;
			return (true);
		}
	}
	return (false);
}

void Channel::printClients( void ) const {
	for (unsigned long i = 0; i < _clients.size(); i++)
	{
		std::cout << " client with address" << _clients[i] << \
					" is in the channel " << _name << std::endl;
	}
}

void Channel::printPrivate( void ) const
{
	std::cout << "channel info:" << std::endl;
	std::cout	<< "	id: " << std::setw(2) << _id \
				<< ", addr: " << std::setw(sizeof(this) + 2) << this \
				<< ", name: " << std::setw(_name.size()) << _name \
				<< ", admin_nick: " << (_admin->getNick().empty() ? "(empty)" : _admin->getNick()) \
				<< ", admin_addr: " << &_admin;

	std::cout << std::endl;
	
	if (!_clients.empty())
		std::cout << "	printing channel clients: " << std::endl;

	for (unsigned long i = 0; i < _clients.size(); i++)
	{
		std::cout << "	";
		_clients[i]->printPrivate();
	}
}

void Channel::removeClient( Client *client )
{	
	std::vector<Client *>::iterator it = _clients.begin();
	for (; it != _clients.end(); it++)
	{
		if (*it == client)
			break ;
	}
	if (it != _clients.end())
		_clients.erase(it);
}
