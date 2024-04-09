#include "Client.hpp"

Client::Client( int fd , int id ) : _fd(fd), _id(id), _user(""), _nick("")
{
}

Client::~Client()
{
}








/* get functions */
int Client::getId( void ) const
{
	return(_id);
}

const std::string &Client::getUser( void ) const
{
	return(_user);
}

const std::string &Client::getNick( void ) const
{
	return(_nick);
}

