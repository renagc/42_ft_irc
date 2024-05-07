#ifndef CHANNEL_HPP
# define CHANNEL_HPP

# include <iostream>
# include <vector>

# include "Client.hpp"

class Client;

class Channel
{
	private:
		int						_id;
		std::string				_name;
		std::vector<Client *>	_clients;
		Client					*_admin;
		
	public:
		/* Constructor*/
		Channel( const std::string &name, Client *admin );
		~Channel();

		/* set funcs */
		void							add( Client *client );

		/* get funcs */
		int								getId( void ) const;
		const std::string				&getName( void ) const;
		const Client					*getAdmin( void ) const;
		std::vector<std::string>		getNicknames( void ) const;
		std::vector<Client *>			getClients( void ) const;

		/* set funcs */
		void							setId( int id );

		bool							findClient( Client *client ) const;
		void							removeClient( Client *client );

		/* debug funcs */
		void							printClients( void ) const;
		void							printPrivate( void ) const;
};

#endif