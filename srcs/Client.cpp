#include "Client.hpp"

Client::Client( int fd , int id, const std::string &host ) : _fd(fd), _id(id), _user(""), _nick(""), _real_name(""), _curr_channel(NULL), _host(host), _logged(false), _registered(false)
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
const std::string &Client::getHost( void ) const { return(_host); }
Channel *Client::getCurrChannel( void ) const { return(_curr_channel); }
const bool &Client::getLogged( void ) const { return(_logged); }
const bool &Client::getRegistered( void ) const { return(_registered); }
const std::string &Client::getRealName( void ) const { return(_real_name); }


/* set functions */
void Client::setNick( const std::string &nick ) { _nick = nick; }
void Client::setUser( const std::string &user ) { _user = user; }
void Client::setLogged( bool logged ) { _logged = logged; }
void Client::setRegistered( bool registered ) { _registered = registered; }
void Client::setRealName( const std::string &real_name ) { _real_name = real_name; }



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

