#include "../configParser.hpp"

// For each server found in the server vector the Config constructor is called
// The constructor sets all the correct values and then the Config object is added to the Config vector
std::vector<Config>	setConfigVector(std::vector<std::vector<std::string> > &serverVector)
{
	std::vector<Config>	configVector;

	for (size_t i = 0; i < serverVector.size(); i++) //ITERATOR
	{
		Config	conf(serverVector[i]);
		configVector.push_back(conf);
	}
	return (configVector);
}


//Searches the config file for 'server {'. If found it searches the matching closing bracket.
//Then it adds al these line into a 'server vector' and returns this vector.
//Linecount is a static variable that keeps track of the last line that is added to a server vector
//Servercount is a static variable that keeps track of the number of server blocks encountered
//When no server is found: if server count is also 0 an error is thrown. Else a empty vector is returned
std::vector<std::string>	findServerBlock(const std::vector<std::string> &vec)
{
	static size_t				lineCount = 0;
	static size_t				serverCount = 0;
	std::vector<std::string>	serverBlock;
	size_t						begin;
	size_t						end;
	int							found = 0; 

	for (; lineCount < vec.size(); lineCount++)
	{
		if (vec[lineCount].find("server {") != std::string::npos)
		{
			begin = lineCount;
			end = findClosingBracket(vec, begin);
			lineCount = end + 1;
			serverCount++;
			found = 1; 
			break ;
		}
	}
	if (found == 0 && serverCount == 0){
		std::cerr << "No server block found in the provided file" << std::endl; //throw
		exit (1);
	}
	if (found == 0)
		return (std::vector<std::string>());
	for (size_t line = begin; line <= end; line++)
		serverBlock.push_back(vec[line]);
	return (serverBlock);
}


//Creates and returnes a vector of string vectors: vector< vector<string> >
//Where each vector<string> is the body of a server block in the config file
std::vector<std::vector<std::string> >	createServerVector(const std::vector<std::string> &file)
{
	std::vector<std::vector<std::string> >	serverVector;
	std::vector<std::string>				serverBlock;

	serverBlock = findServerBlock(file);
	while (!serverBlock.empty())
	{
		serverVector.push_back(serverBlock);
		serverBlock = findServerBlock(file);
	}
	if (serverVector.empty()){
		std::cerr << "No server block found in the provided file" << std::endl; //overbodig wsl en throw
		exit(1);
	}
	return (serverVector);
}


//Puts all the lines from the config file as strings in a vector<string>
//When a '#' or ';' character is found the rest of the line is seen as a comment, so the line gets truncated
//Empty lines are skipped
const std::vector<std::string>	getFileVector(const std::string &fileName)
{
	std::ifstream	configFile(fileName);
	if (!configFile) {
		std::cerr << "Cannot open file: " << fileName << std::endl; //hier throw gaan gebruiken
		exit(1);
	}
	std::vector<std::string>	confVector;
	std::string	line;
	while (getline(configFile, line))
	{
		if (line.empty())
			continue;
		if (line.at(0) == '#')
			continue;
		line = truncateString(line, '#');
		line = truncateString(line, ';');
		confVector.push_back(line);
	}
	return (confVector);
}
