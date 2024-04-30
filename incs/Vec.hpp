#ifndef VEC_HPP
# define VEC_HPP

# include <vector>
# include <iostream>

template<typename T>
class Vec : public std::vector<T>
{
	public:
		Vec();
		~Vec();

		typename std::vector<T>::iterator	find( const T &value );
		// bool								remove( const T &value );
};

template<typename T>
Vec<T>::Vec() : std::vector<T>()
{
}

template<typename T>
Vec<T>::~Vec()
{
}

template<typename T>
typename std::vector<T>::iterator Vec<T>::find( const T &value )
{
	typename std::vector<T>::iterator it;
	for (it = this->begin(); it != this->end(); it++)
	{
		if (*it == value)
			break ;
	}
	return (it);
}

// template<typename T>
// bool Vec<T>::remove( const T &value )
// {
// 	if (this->find(value) != this->end())
// 	{
// 		this->erase(this->find(value));
// 		return (true);
// 	}
// 	return (false);
// }


#endif