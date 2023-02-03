// TODO: create << operator overloads for the ParsedConfigFile and Config class

#include <iostream>
#include <vector>

//prints a std::vector<std::string> string by string
void	print_string_vector(std::vector<std::string> &vec)
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
void	print_server_vector(std::vector<std::vector<std::string> > &vec)
{
	for (size_t i = 0; i < vec.size(); i++)
	{
		std::cout << "	*** SERVER " << i + 1 << " ***" << std::endl;
		print_string_vector(vec[i]);
		std::cout << std::endl;
	}
}
