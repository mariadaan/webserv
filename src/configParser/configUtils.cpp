#include "../configParser.hpp"

//Checks if the word found is one of the keywords
size_t	determine_if_keyword(const std::string &word)
{
	if (word == "listen")
		return (LISTEN);
	else if (word == "server_name")
		return (SERVER_NAME);
	else if (word == "root")
		return (ROOT);
	else if (word == "location")
		return (LOCATION);
	else if (word == "client_max_body_size")
		return (MAX_SIZE);
	else if (word == "error_page")
		return (ERROR_PAGE);
	else if (word == "cgi")
		return (CGI);
	return (0);
}


//Function that gets called in Config constructor when a 'location' keyword is found
//Searches and returns the body of the location
std::vector<std::string>	return_location_body(std::vector<std::string> &server_vector, size_t i, size_t end)
{
	std::vector<std::string>	location_body;
	while (i < end){
		location_body.push_back(server_vector[i]);
		i++;
	}
	return (location_body);
}


//converts string to unsigned int
//if input is not only digits 0 is returned
unsigned int	string_to_unsigned(std::string &word)
{
	for (size_t i = 0; i < word.length(); i++)
	{
		if (!isdigit(word[i]))
			return (0);
	}
	return (std::stoul(word));
}


//If a second word is found it is converted to an unsigned int (if possible)
//If yes: the unsigned is set either to the 'port' value or 'max_body_size'
void	value_to_unsigned(Config &object, std::string &line, size_t &enum_value)
{
	std::string		word = get_second_word(line);
	unsigned int	value = string_to_unsigned(word);
	if (value != 0)
	{
		if (enum_value == LISTEN)
			object.set_port(value);
		else if (enum_value == MAX_SIZE)
			object.set_max_size(value);
	}
}


//When 'server names' is found this function is used to search all names and puts them in a string vector 
void	value_to_string_vector(Config &object, std::string &line)
{
	std::vector<std::string>	server_names;
	std::string					name;

	std::stringstream			ss(line);
	ss >> name; 				//first word
	while (ss >> name)
		server_names.push_back(name);
	if (server_names.empty())
		return ;
	object.set_server_names(server_names);
}


//sets the value for root and cgi if a second word is encountered
void	value_to_string(Config &object, std::string &line, size_t &enum_value)
{
	std::string	value = get_second_word(line);
	if (!value.empty())
	{
		if (enum_value == ROOT)
			object.set_root(value);
		else if (enum_value == CGI)
			object.set_cgi(value);
	}
}


//If  "error_page  404 path/name.html" is found in config file, the path value is changed from default to 'path/name.html'
void	value_to_error(Config &object, std::string &line)
{
	std::string			value;
	std::string			error_code_string;
	int					error_code;

	error_code_string = get_second_word(line);
	if (error_code_string.empty()){
		std::cerr << "No error code provided in config file" << std::endl;
		exit(EXIT_FAILURE) ;
	}
	error_code = stoi(error_code_string);
	if (error_code == 0){
		std::cerr << "No correct error code provided in config file" << std::endl;
		exit(EXIT_FAILURE) ;
	}
	value = get_third_word(line);
	if (value.empty()){
		std::cerr << "No error page provided for error code '" << error_code << "' in config file" << std::endl;
		exit(EXIT_FAILURE) ;
	}
	object.set_error_page(error_code, value);
}


//Creates a map<string,bool> for the methods map in the Location class
//All the values are iniated to be false at first
std::map<std::string, bool>	return_false_methods_map(void)
{
	std::map<std::string, bool>	methods;
	methods["GET"] = false;
	methods["POST"] = false;
	methods["DELETE"] = false;
	methods["PUT"] = false;
	methods["PATCH"] = false;
	return (methods);
}


//Creates a map<int,string> where all error codes are set to default path
std::map<int,std::string>	return_default_error_map(void)
{
	std::map<int,std::string> error;
	error[400] = "default/error/400.html";
	error[403] = "default/error/403.html";
	error[404] = "default/error/404.html";
	error[405] = "default/error/405.html";
	error[413] = "default/error/413.html";
	error[418] = "default/error/418.html";
	error[500] = "default/error/500.html";
	error[501] = "default/error/501.html";
	error[505] = "default/error/505.html";
	return (error);
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

//Returns third word of string line, if no 3rd word is found, a empty string is returned
std::string	get_third_word(std::string &line)
{
	std::stringstream	ss(line);
	std::string			word;

	ss >> word; //1st
	word = "";
	ss >> word; //2nd
	word = "";
	ss >> word; //3rd
	return (word);
}

//Returns second word of string line, if no 2nd word is found, a empty string is returned
std::string	get_second_word(std::string &line)
{
	std::stringstream	ss(line);
	std::string			word;

	ss >> word; //get first word
	word = "";
	ss >> word; //get second word
	return (word);
}


//Finds first word of string 'line' and returns this word
std::string find_first_word(std::string &line) 
{
    std::stringstream ss(line);
    std::string first_word;
    ss >> first_word;
    return first_word;
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

