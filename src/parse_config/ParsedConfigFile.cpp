#include "ParsedConfigFile.hpp"

ParsedConfigFile::ParsedConfigFile(char const *conf_filename) {
	this->parse_config(conf_filename);
}

void	ParsedConfigFile::parse_config(char const *conf_filename)
{
	std::vector<std::string> lines;
	lines = file_to_lines(conf_filename);

	if (!check_brackets(lines))
		throw std::runtime_error("Found non matching bracket(s) in '" + (std::string)conf_filename + "' file");
	std::vector<std::vector<std::string> >	server_vector;
	server_vector = create_server_vector(lines);
	this->server_blocks = set_config_vector(server_vector);
}

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

//Puts all the lines from the config file as strings in a vector<string>
//When a '#' or ';' character is found the rest of the line is seen as a comment, so the line gets truncated
//Empty lines are skipped
const std::vector<std::string>	file_to_lines(const std::string &file_name)
{
	std::ifstream	config_file(file_name);
	if (!config_file)
		throw std::runtime_error( "Cannot open file: " + file_name);
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

// Function to check if each bracket '{' has a matching closing bracket '}'
bool	check_brackets(const std::vector<std::string> &vec)
{
	int	open_brackets = 0;
	for (size_t line = 0; line < vec.size(); line++)
	{
		for (size_t pos = 0; pos < vec[line].length(); pos++)
		{
			if (vec[line].at(pos) == '{')
				open_brackets++;
			else if (vec[line].at(pos) == '}'){
				if (open_brackets == 0)
					return (false);
				open_brackets--;
			}
		}
	}
	if (open_brackets != 0)
		return (false);
	return (true);
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
	if (server_vector.empty())
		throw std::runtime_error("No server block found in the provided file");
	return (server_vector);
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
	if (found == 0 && server_count == 0)
		throw std::runtime_error("No server block found in the provided file");
	if (found == 0)
		return (std::vector<std::string>());
	for (size_t line = begin; line <= end; line++)
		server_block.push_back(vec[line]);
	return (server_block);
}


//Searches on which line number the closing bracket '}' is found
//'OpenBrackets' count is used for the server block because location also has open brackets
size_t	find_closing_bracket(const std::vector<std::string> &vec, size_t line)
{
	line++;
	int	open_brackets = 1;
	int	check = 0;
	for (; line < vec.size(); line++)
	{
		for (size_t pos = 0; pos < vec[line].length(); pos++)
		{
			if (vec[line].at(pos) == '{')
				open_brackets++;
			else if (vec[line].at(pos) == '}')
				open_brackets--;
			if (open_brackets == 0){
				check = 1;
				break ;
			}
		}
		if (check == 1)
			break ;
	}
	return (line);
}

//Function that truncates the string if a char c is found
//If not found the original string is not truncated and returned
std::string	truncate_string(const std::string &str, char c)
{
	size_t	position = str.find(c);
	if (position != std::string::npos)
		return (str.substr(0, position));
	return (str);
}
