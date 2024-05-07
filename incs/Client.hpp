#ifndef CLIENT_HPP
# define CLIENT_HPP

# include <iostream>
# include <iomanip>
# include <vector>
# include "Channel.hpp"

class Channel;

class Client
{
	private:
		int				_fd;
		int				_id;
		std::string		_user;
		std::string		_nick;
		Channel			*_curr_channel;
		
	public:
		/* Constructor*/
		Client( int fd , int id );
		~Client();

		/* get funcs */
		int							getFd( void ) const;
		int							getId( void ) const;
		const std::string			&getUser( void ) const;
		const std::string			&getNick( void ) const;
		Channel						*getCurrChannel( void ) const;
		
		/* set funcs */
		void						setCurrChannel( Channel *channel );

		/* logs */
		void						printPrivate( void ) const;
};

#endif