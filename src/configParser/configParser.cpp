#include "../configParser.hpp"

// For each server found in the server vector the Config constructor is called
// The constructor sets all the correct values and then the Config object is added to the Config vector
std::vector<Config>	set_config_vector(std::vector<std::vector<std::string> > &server_vector)
{
	std::vector<Config>	config_vector;

	for (size_t i = 0; i < server_vector.size(); i++) //ITERATOR
	{
		Config	conf(server_vector[i]);
		config_vector.push_back(conf);
	}
	return (config_vector);
}


//Searches the config file for 'server {'. If found it searches the matching closing bracket.
//Then it adds al these line into a 'server vector' and returns this vector.
//Linecount is a static variable that keeps track of the last line that is added to a server vector
//Servercount is a static variable that keeps track of the number of server blocks encountered
//When no server is found: if server count is also 0 an error is thrown. Else a empty vector is returned
std::vector<std::string>	find_server_block(const std::vector<std::string> &vec)
{
	static size_t				line_count = 0;
	static size_t				server_count = 0;
	std::vector<std::string>	server_block;
	size_t						begin;
	size_t						end;
	int							found = 0; 

	for (; line_count < vec.size(); line_count++)
	{
		if (vec[line_count].find("server {") != std::string::npos)
		{
			begin = line_count;
			end = find_closing_bracket(vec, begin);
			line_count = end + 1;
			server_count++;
			found = 1; 
			break ;
		}
	}
	if (found == 0 && server_count == 0){
		std::cerr << "No server block found in the provided file" << std::endl;
		exit (EXIT_FAILURE);
	}
	if (found == 0)
		return (std::vector<std::string>());
	for (size_t line = begin; line <= end; line++)
		server_block.push_back(vec[line]);
	return (server_block);
}


//Creates and returnes a vector of string vectors: vector< vector<string> >
//Where each vector<string> is the body of a server block in the config file
std::vector<std::vector<std::string> >	create_server_vector(const std::vector<std::string> &file)
{
	std::vector<std::vector<std::string> >	server_vector;
	std::vector<std::string>				server_block;

	server_block = find_server_block(file);
	while (!server_block.empty())
	{
		server_vector.push_back(server_block);
		server_block = find_server_block(file);
	}
	if (server_vector.empty()){
		std::cerr << "No server block found in the provided file" << std::endl;
		exit(EXIT_FAILURE);
	}
	return (server_vector);
}


//Puts all the lines from the config file as strings in a vector<string>
//When a '#' or ';' character is found the rest of the line is seen as a comment, so the line gets truncated
//Empty lines are skipped
const std::vector<std::string>	get_file_vector(const std::string &file_name)
{
	std::ifstream	config_file(file_name);
	if (!config_file) {
		std::cerr << "Cannot open file: " << file_name << std::endl;
		exit(EXIT_FAILURE);
	}
	std::vector<std::string>	conf_vector;
	std::string	line;
	while (getline(config_file, line))
	{
		if (line.empty())
			continue;
		if (line.at(0) == '#')
			continue;
		line = truncate_string(line, '#');
		line = truncate_string(line, ';');
		conf_vector.push_back(line);
	}
	return (conf_vector);
}
