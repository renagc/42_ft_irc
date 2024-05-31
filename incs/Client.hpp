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
		std::string		_real_name;
		Channel			*_curr_channel;
		std::string		_host;
		bool			_logged;
		bool			_registered;
		
	public:
		/* Constructor*/
		Client( int fd , int id, const std::string &host );
		~Client();

		/* get funcs */
		int							getFd( void ) const;
		int							getId( void ) const;
		const std::string			&getUser( void ) const;
		const std::string			&getNick( void ) const;
		Channel						*getCurrChannel( void ) const;
		const std::string			&getHost( void ) const;
		const bool					&getLogged( void ) const;
		const bool					&getRegistered( void ) const;
		const std::string			&getRealName( void ) const;
		
		/* set funcs */
		void						setCurrChannel( Channel *channel );
		void						setNick( const std::string &nick );
		void						setUser( const std::string &user );
		void						setLogged( bool logged );
		void						setRegistered( bool registered );
		void						setRealName( const std::string &real_name );

		/* logs */
		void						printPrivate( void ) const;
};

#endif