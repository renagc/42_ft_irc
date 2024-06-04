#ifndef CHANNEL_HPP
# define CHANNEL_HPP

# include <iostream>
# include <vector>
# include <algorithm>

# include "Client.hpp"
# include "Utils.hpp"

class Client;

class Channel
{
	private:
		int						_id;
		std::string				_name;
		std::vector<Client *>	_clients;
		std::vector<Client *>	_clients_kicked;
		std::string				_pw;
		int						_limit;
		std::string				_topic;

		/* flags */
		std::vector<Client *>	_operators;
		bool					_i; // Set/remove Invite-only channel
		bool					_t; // Set/remove the restrictions of the TOPIC command to channel operators
		bool					_k; // Set/remove the channel key (password)
		bool					_l; // Set/remove the user limit to channel
		
	public:
		/* Constructor*/
		Channel( const std::string &name, Client *admin );
		~Channel();

		/* set funcs */
		void							add( Client *client );

		/* get funcs */
		int								getId( void ) const;
		const std::string				&getName( void ) const;
		std::vector<std::string>		getNicknames( void ) const;
		std::vector<Client *>			getClients( void ) const;
		std::vector<Client *>			getOperators( void ) const;
		bool							getI( void ) const;
		bool							getT( void ) const;
		bool							getK( void ) const;
		bool							getL( void ) const;
		const std::string				&getTopic( void ) const;
		const int						&getLimit( void ) const;
		const std::string				&getPw( void ) const;
		std::string						getUsers( void );
		std::string						getMode( void );

		/* set funcs */
		void							setId( int id );
		void							setT( bool i );
		void							setI( bool t );
		void							setK( bool k );
		void							setL( bool l );
		void							setTopic( const std::string &topic );
		void							setLimit( int limit );
		void							setPw( const std::string &pw );
		void							addOperator( Client *client );
		void							addKicked( Client *client );
		void							removeOperator( Client *client );

		bool							findClient( Client *client ) const;
		void							removeClient( Client *client );

		/* debug funcs */
		void							printClients( void ) const;
		void							printPrivate( void ) const;
		bool							isOperator(Client *client);
		bool							isKicked(Client *client);
};

#endif