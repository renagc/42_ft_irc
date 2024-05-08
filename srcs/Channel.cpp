#include "../incs/Channel.hpp"

/* constructors */
Channel::Channel( const std::string &name, Client *admin ) : _name(name)
{
	_operators.push_back(admin);
	_clients.push_back(admin);
	_pw = "";
	_i = false;
	_t = false;
	_k = false;
	_l = false;
}

Channel::~Channel() {}

/* add */
void Channel::add( Client *client )
{ 
	_clients.push_back(client);
}

/* accessors */
const std::string &Channel::getName( void ) const { return(_name); }

std::vector<std::string>	Channel::getNicknames( void ) const {
	std::vector<std::string> nicks;

	for(unsigned long i = 0; i < _clients.size(); i++)
		nicks.push_back(_clients[i]->getNick());

	return (nicks);
}

std::vector<Client *> Channel::getClients( void ) const { return(_clients); }
bool	Channel::findClient( Client *client ) const
{
	for (unsigned long i = 0; i < _clients.size(); i++)
	{
		if (_clients[i] == client)
			return (true);
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
				<< ", name: " << std::setw(_name.size()) << _name;

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

void Channel::setId( int id ){ _id = id; }
int Channel::getId( void ) const { return(_id); }

std::vector<Client *> Channel::getOperators( void ) const { return(_operators); }
void Channel::addOperator( Client *client ) { _operators.push_back(client); }
void Channel::removeOperator( Client *client )
{
	std::vector<Client *>::iterator it;
	for (it = _operators.begin(); it != _operators.end(); it++)
	{
		if (client == *it)
			break;
	}
	_operators.erase(it);
}

const bool &Channel::getI( void ) const { return(_i); }
const bool &Channel::getT( void ) const { return(_t); }
const bool &Channel::getK( void ) const { return(_k); }
const bool &Channel::getL( void ) const { return(_l); }

void Channel::setT( const bool &i ) { _i = i; }
void Channel::setI( const bool &t ) { _t = t; }
void Channel::setK( const bool &k ) { _k = k; }
void Channel::setL( const bool &l ) { _l = l; }