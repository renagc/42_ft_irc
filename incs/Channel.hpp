#ifndef CHANNEL_HPP
# define CHANNEL_HPP

# include <iostream>
# include <vector>

# include "Client.hpp"

class Channel
{
	private:
		std::string				_name;
		std::vector<Client *>	_clients;
		Client					*_admin;
		
	public:
		/* Constructor*/
		Channel( const std::string &name, Client *admin );
		~Channel();

		/* get funcs */
		const std::string				&getName( void ) const;
		const Client					*getAdmin( void ) const;
		std::vector<std::string>		getNicknames( void ) const;
};

#endif