#include "Client.hpp"

Client::Client( int fd , int id ) : _fd(fd), _id(id), _user(""), _nick(""), _curr_channel(NULL)
{
}

Client::~Client()
{
}






/* get functions */
int Client::getFd( void ) const { return(_fd); }
int Client::getId( void ) const { return(_id); }
const std::string &Client::getUser( void ) const { return(_user); }
const std::string &Client::getNick( void ) const { return(_nick); }
Channel *Client::getCurrChannel( void ) const { return(_curr_channel); }



void Client::setCurrChannel( Channel *channel ) { _curr_channel = channel; }



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

