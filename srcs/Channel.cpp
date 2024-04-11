#include "Channel.hpp"

/* constructors */
Channel::Channel( const std::string &name, Client *admin ) : _name(name), _admin(admin) {}

Channel::~Channel() {}



/* accessors */

const std::string &Channel::getName( void ) const { return(_name); }

const Client *Channel::getAdmin( void ) const { return(_admin); }

std::vector<std::string>	Channel::getNicknames( void ) const {
	std::vector<std::string> nicks;

	for(unsigned long i = 0; i < _clients.size(); i++)
		nicks.push_back(_clients[i]->getNick());

	return (nicks);
}