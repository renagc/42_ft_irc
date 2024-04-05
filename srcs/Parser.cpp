#include "Parser.hpp"

int Parser::port(char *p)
{
	for (int i = 0; p[i]; i++)
		if (!std::isdigit(p[i]))
			throw ParserPortException();
	return(std::atoi(p));
}

