#include "../configParser.hpp"

//Checks if the word found is one of the keywords
size_t	determineIfKeyword(const std::string &word)
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
std::vector<std::string>	returnLocationBody(std::vector<std::string> &serverVector, size_t i, size_t end)
{
	std::vector<std::string>	locationBody;
	while (i < end){
		locationBody.push_back(serverVector[i]);
		i++;
	}
	return (locationBody);
}


//converts string to unsigned int
//if input is not only digits 0 is returned
unsigned int	stringToUnsigned(std::string &word)
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
void	valueToUnsigned(Config &object, std::string &line, size_t &enumValue)
{
	std::string		word = getSecondWord(line);
	unsigned int	value = stringToUnsigned(word);
	if (value != 0)
	{
		if (enumValue == LISTEN)
			object.setPort(value);
		else if (enumValue == MAX_SIZE)
			object.setMaxSize(value);
	}
}


//When 'server names' is found this function is used to search all names and puts them in a string vector 
void	valueToStringVector(Config &object, std::string &line)
{
	std::vector<std::string>	serverNames;
	std::string					name;

	std::stringstream			ss(line);
	ss >> name; 				//first word
	while (ss >> name)
		serverNames.push_back(name);
	if (serverNames.empty())
		return ;
	object.setServerNames(serverNames);
}


//sets the value for root and cgi if a second word is encountered
void	valueToString(Config &object, std::string &line, size_t &enumValue)
{
	std::string	value = getSecondWord(line);
	if (!value.empty())
	{
		if (enumValue == ROOT)
			object.setRoot(value);
		else if (enumValue == CGI)
			object.setCgi(value);
	}
}


//If  "error_page  404 path/name.html" is found in config file, the path value is changed from default to 'path/name.html'
void	valueToError(Config &object, std::string &line)
{
	std::string			value;
	std::string			errorCodeString;
	int					errorCode;

	errorCodeString = getSecondWord(line);
	if (errorCodeString.empty()){
		std::cerr << "No error code provided in config file" << std::endl;
		return ;
	}
	errorCode = stoi(errorCodeString);
	if (errorCode == 0){
		std::cerr << "No correct error code provided in config file" << std::endl;
		return ;
	}
	value = getThirdWord(line);
	if (value.empty()){
		std::cerr << "No error page provided for error code '" << errorCode << "' in config file" << std::endl;
		return ;
	}
	object.setErrorPage(errorCode, value);
}


//Creates a map<string,bool> for the methods map in the Location class
//All the values are iniated to be false at first
std::map<std::string, bool>	returnFalseMethodsMap(void)
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
std::map<int,std::string>	returnDefaultErrormap(void)
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
std::string	truncateString(const std::string &str, char c)
{
	size_t	position = str.find(c);
	if (position != std::string::npos)
		return (str.substr(0, position));
	return (str);
}

//Returns third word of string line, if no 3rd word is found, a empty string is returned
std::string	getThirdWord(std::string &line)
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
std::string	getSecondWord(std::string &line)
{
	std::stringstream	ss(line);
	std::string			word;

	ss >> word; //get first word
	word = "";
	ss >> word; //get second word
	return (word);
}


//Finds first word of string 'line' and returns this word
std::string findFirstWord(std::string &line) 
{
    std::stringstream ss(line);
    std::string firstWord;
    ss >> firstWord;
    return firstWord;
}


// Function to check if each bracket '{' has a matching closing bracket '}'
bool	checkBrackets(const std::vector<std::string> &vec)
{
	int	openBrackets = 0;
	for (size_t line = 0; line < vec.size(); line++)
	{
		for (size_t pos = 0; pos < vec[line].length(); pos++)
		{
			if (vec[line].at(pos) == '{')
				openBrackets++;
			else if (vec[line].at(pos) == '}'){
				if (openBrackets == 0)
					return (false);
				openBrackets--;
			}
		}
	}
	if (openBrackets != 0)
		return (false);
	return (true);
}

//Searches on which line number the closing bracket '}' is found
//'OpenBrackets' count is used for the server block because location also has open brackets
size_t	findClosingBracket(const std::vector<std::string> &vec, size_t line)
{
	line++;
	int	openBrackets = 1;
	int	check = 0;
	for (; line < vec.size(); line++)
	{
		for (size_t pos = 0; pos < vec[line].length(); pos++)
		{
			if (vec[line].at(pos) == '{')
				openBrackets++;
			else if (vec[line].at(pos) == '}')
				openBrackets--;
			if (openBrackets == 0){
				check = 1;
				break ;
			}
		}
		if (check == 1)
			break ;
	}
	return (line);
}

