#ifndef UTILS_HPP
#define UTILS_HPP

# include <iostream>
# include <vector>

std::vector<std::string>	split( const std::string &str, const std::string &del );
void						printVector( const std::vector<std::string> &vec );
void						printLocalTime( void );
void						log( const std::string &str );
void						log( const std::string &str, int id );
void						log( const std::string &str, const std::string &from, const std::string &to );


#endif // UTILS_HPP