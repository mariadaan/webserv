#include "../configParser.hpp"

std::vector<Config>	parse_config(char const *conf_file)
{
	std::vector<std::string> fileVector;
	fileVector = getFileVector(conf_file);
	// printStringVector(fileVector);

	if (!checkBrackets(fileVector)) {
		std::cerr << "Found non matching bracket(s) in '" << conf_file << "' file" << std::endl; //throw
		exit(1);
	}

	std::vector<std::vector<std::string> >	serverVector;
	serverVector = createServerVector(fileVector);
	// printServerVector(serverVector);

	std::vector<Config>	configVector;
	configVector = setConfigVector(serverVector);	
	// for (size_t i = 0; i < configVector.size(); i++)
	// {
	// 	configVector[i].printConfigClass();
	// 	std::cout << "-------------------------------------------" << std::endl;
	// }
	return (configVector);
}

