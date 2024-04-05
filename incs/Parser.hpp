#include <iostream>
#include <cctype>
#include <cstdlib>
#include <exception>

class Parser
{
	private:
		// int			_port;
		// std::string _password;

		Parser();
		static bool alldigits( void );

	public:
		virtual				~Parser();
		static int			port( char *p );
		static std::string	password( char *pw );

		class ParserPortException : public std::exception
		{
			public:
				virtual const char *what() const throw() { return ("Error on port"); }
		};
		class ArgumentsException : public std::exception
		{
			public:
				virtual const char *what() const throw() { return ("Wrong number of arguments.\n\nUsage: ./ircserv <port> <password>"); }
		};
};