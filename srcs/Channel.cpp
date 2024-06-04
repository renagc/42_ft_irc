#include "Channel.hpp"

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
	_limit = -1;
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
void Channel::addOperator( Client *client )
{
	std::vector<Client *>::iterator it = _operators.begin();
	for (; it != _operators.end(); it++)
	{
		if (*it == client)
			return ;
	}
	_operators.push_back(client);
}
void Channel::addKicked( Client *client ) { _clients_kicked.push_back(client); }
void Channel::removeOperator( Client *client )
{
	std::vector<Client *>::iterator it;
	for (it = _operators.begin(); it != _operators.end(); it++)
	{
		if (client == *it)
			break;
	}
	if (it != _operators.end())
		_operators.erase(it);
}

bool Channel::getI( void ) const { return(_i); }
bool Channel::getT( void ) const { return(_t); }
bool Channel::getK( void ) const { return(_k); }
bool Channel::getL( void ) const { return(_l); }
const std::string &Channel::getTopic( void ) const { return(_topic); }
const int &Channel::getLimit( void ) const { return(_limit); }
const std::string &Channel::getPw( void ) const { return(_pw); }

bool Channel::isOperator(Client *client)
{
	for (unsigned long i = 0; i < _operators.size(); i++)
	{
		if (_operators[i] == client)
			return (true);
	}
	return (false);
}

bool Channel::isKicked(Client *client)
{
	for (unsigned long i = 0; i < _clients_kicked.size(); i++)
	{
		if (_clients_kicked[i] == client)
			return (true);
	}
	return (false);
}

std::string Channel::getUsers( void )
{
	std::string operators;
	for (unsigned long i = 0; i < _operators.size(); i++)
	{
		operators += "@" + _operators[i]->getNick();
		if (i + 1 < _operators.size())
			operators += " ";
	}
	std::string clients;
	for (unsigned long i = 0; i < _clients.size(); i++)
	{
		if (std::find(_operators.begin(), _operators.end(), _clients[i]) != _operators.end())
			continue ;
		clients += _clients[i]->getNick();
		if (i + 1 < _clients.size())
			clients += " ";
	}
	if (!operators.empty() && !clients.empty())
		return(clients + " " + operators);
	else if (operators.empty())
		return(clients);
	else
		return(operators);
}

std::string Channel::getMode( void )
{
	std::string mode = "+";
	if (_i)
		mode += "i";
	if (_t)
		mode += "t";
	if (_k)
		mode += "k";
	if (_l)
	{
		mode += "l";
		if (_limit != -1)
			mode += " " + itoa(_limit);
	}
	return (mode);
}

void Channel::setI( bool i ) { _i = i; }
void Channel::setT( bool t ) { _t = t; }
void Channel::setK( bool k ) { _k = k; }
void Channel::setL( bool l ) { _l = l; }
void Channel::setTopic( const std::string &topic ) { _topic = topic; }
void Channel::setLimit( int limit ) { _limit = limit; }
void Channel::setPw( const std::string &pw ) { _pw = pw; }