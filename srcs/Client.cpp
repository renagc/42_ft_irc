#include "Client.hpp"

Client::Client( int fd , int id ) : _fd(fd), _id(id), _user(""), _nick("")
{
}

Client::~Client()
{
}






/* get functions */
int Client::getId( void ) const { return(_id); }
const std::string &Client::getUser( void ) const { return(_user); }
const std::string &Client::getNick( void ) const { return(_nick); }
void Client::printPrivate( void ) const
{
	std::cout << "client info:" << std::endl;
	std::cout	<< "	fd: " << std::setw(2) << _fd \
				<< ", id: " << std::setw(2) << _id \
				<< ", addr: " << std::setw(sizeof(this) + 2) << this \
				<< ", user: " << (_user.empty() ? "(empty)" : _user) \
				<< ", nick: " << (_nick.empty() ? "(empty)" : _nick) \
				<< std::endl;
}

