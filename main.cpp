#include <iostream>
#include <vector>

int main (void)
{
	std::vector<std::string>	ola;

	ola.push_back("ola");
	ola.push_back("adeus");
	ola.push_back("!");

	std::vector<std::string>::iterator	it = ola.begin();
	for (; it != ola.end(); it++)
	{
		std::cout << *it << std::endl;
	}
	--it;
	ola.erase(it);
	for (it = ola.begin(); it != ola.end(); it++)
	{
		std::cout << *it << std::endl;
	}
	return (0);
}