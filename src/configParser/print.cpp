#include "../configParser.hpp"

//prints a std::vector<std::string> string by string
void	printStringVector(std::vector<std::string> &vec)
{
	size_t	line_number = 1;
	for (size_t i = 0; i < vec.size(); i++)
	{
		std::cout << line_number << ": " << vec[i] << std::endl;
		line_number++;
	}
}

//Prints out a server vector
//in other words a vector of string vectors: vector< vector<string> >
void	printServerVector(std::vector<std::vector<std::string> > &vec)
{
	for (size_t i = 0; i < vec.size(); i++)
	{
		std::cout << "	*** SERVER " << i + 1 << " ***" << std::endl;
		printStringVector(vec[i]);
		std::cout << std::endl;
	}
}
