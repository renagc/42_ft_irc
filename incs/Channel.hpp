#ifndef CHANNEL_HPP
# define CHANNEL_HPP

# include <iostream>
# include <vector>

# include "Client.hpp"

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
		const std::string				&getName( void ) const;
		const Client					*getAdmin( void ) const;
		std::vector<std::string>		getNicknames( void ) const;
		std::vector<Client *>			getClients( void ) const;

		bool							findClient( Client *client ) const;
		void							removeClient( Client *client );

		/* debug funcs */
		void							printClients( void ) const;
		void							printPrivate( void ) const;
};

#endif