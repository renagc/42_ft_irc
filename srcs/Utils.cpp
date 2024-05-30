#include "Utils.hpp"

// this function will split by the delimiter string, not seprated caracters on delimiter
std::vector<std::string> split( const std::string &str, const std::string &del )
{
	std::string							new_parse = str;
	std::vector<std::string>			splitted;

	size_t	end = new_parse.find(del);
	while (end != std::string::npos)
	{	
		splitted.push_back(new_parse.substr(0, end));
		new_parse = new_parse.substr(end + del.size(), new_parse.size() - end - del.size());
		while (new_parse.find(del) == 0)
			new_parse = new_parse.substr(del.size(), new_parse.size() - del.size());
		end = new_parse.find(del);
	}
	if (new_parse.size() > 0)
		splitted.push_back(new_parse);
	return (splitted);	
}

void printVector( const std::vector<std::string> &vec )
{
	std::cout << "printing vector:" << std::endl;
	std::vector<std::string>::const_iterator it = vec.begin();
	for (; it != vec.end(); it++)
		std::cout << "	 line " << it - vec.begin() << " " << *it << std::endl;
}

void printLocalTime( void )
{
	time_t rawtime;
	struct tm *timeinfo;

	time(&rawtime);
	timeinfo = localtime(&rawtime);
	std::cout << timeinfo->tm_year + 1900 << "-" << \
				timeinfo->tm_mon + 1 << "-" << \
				timeinfo->tm_mday << " " << \
				timeinfo->tm_zone << " " << \
				timeinfo->tm_hour << ":" << \
				timeinfo->tm_min << ":" << \
				timeinfo->tm_sec;
}

void log( const std::string &str )
{
	printLocalTime();
	std::cout << " | " << "server: " << str << std::endl;
}

void log( const std::string &str, int id )
{
	printLocalTime();
	std::cout << " | " << "server: " << str << " {id: " << id <<  "}" << std::endl;
}

void log( const std::string &str, const std::string &from, const std::string &to )
{
	printLocalTime();
	std::cout << " | " << "server: " << str << " from: " << from << ", to: " << to <<std::endl;
}

std::string joinString( const std::vector<std::string> &vec, std::vector<std::string>::const_iterator pos )
{
	std::string									joined;
	for (; pos != vec.end(); pos++)
	{
		joined.append(*pos);
		if (pos + 1 != vec.end())
			joined.append(" ");
	}
	return (joined);
}

std::vector<std::string> strtov(int number, ...)
{
	std::vector<std::string> vec;
	va_list ap;
	va_start(ap, number);
	for (int i = 0; i < number; i++)
		vec.push_back(va_arg(ap, const char *));
	va_end(ap);
	return (vec);
}