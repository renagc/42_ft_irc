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

		const bool						&getI( void ) const;
		const bool						&getT( void ) const;
		const bool						&getK( void ) const;
		const bool						&getL( void ) const;
		const int						&getLimit( void ) const;
		void							addOperator( Client *client );
		void							addClient( Client *client );
		void							removeOperator( Client *client );

		void							setT( const bool &i );
		void							setI( const bool &t );
		void							setK( const bool &k );
		void							setL( const bool &l );
		void							setTopic(const std::string topic);
		void							setPW(const std::string pw);
		void							setLimit(const int limit);

		/* set funcs */
		void							setId( int id );

		bool							findClient( Client *client ) const;
		void							removeClient( Client *client );

		/* debug funcs */
		void							printClients( void ) const;
		void							printPrivate( void ) const;
};

#endif