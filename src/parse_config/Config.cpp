#include "Config.hpp"
#include "ParsedConfigFile.hpp"
#include "Logger.hpp"
// #include "configParser.hpp"


//                 **********************************************
//                 **************** CONFIG CLASS ****************
//                 **********************************************


// -------------------------------- CONSTRUCTORS ---------------------------------


//No dynamically allocated memmory in the class so destructor is empty
Config::~Config(){}


//Copy assignent operator using the copy and swap idiom
//Note that the object 'other' is given as value instead of reference
//Therefore the object gets copied and it does not touch the original object
//The copy and swap idiom has multiple benifits (look up if interested)
Config& Config::operator=(Config other)
{
	this->_port = other._port;
	this->_max_size = other._max_size;
	this->_server_names = other._server_names;
	this->_root = other._root;
	this->_cgi = other._cgi;
	this->_locations = other._locations;
	this->_error_pages = other._error_pages;
	return *this;
}

Config::Config(std::vector<std::string> &server_vector)
	: _max_size(0), _error_pages(return_default_error_map()), port_defined(false)
{
	Config::conf_parser	enum_value;

	for (size_t i = 0; i < server_vector.size(); i++)
	{
		std::string word = find_first_word(server_vector[i]);
		enum_value = determine_if_keyword(word);
		if (enum_value == NOT_A_KEYWORD)
			continue ;
		else if (enum_value == LOCATION)
		{
			std::string	location_name = get_second_word(server_vector[i]);
			if (location_name.empty())
				continue ;
			size_t end = find_closing_bracket(server_vector, i);
			std::vector<std::string> location_body = return_location_body(server_vector, i, end);
			Location location(location_body);
			this->set_locations(location_name, location);
			while (i < end)
				i++;
		}
		else
			call_keyword_function(enum_value, server_vector[i]);
	}
	for (std::map<std::string,Location>::iterator it = _locations.begin(); it != _locations.end(); it++){
		if (it->second.get_max_size() == 0 && this->_max_size != 0)
			it->second.set_max_size(this->_max_size);
	}
	if (_server_names.empty())
		throw std::runtime_error("No server name(s) defined in the server block listening on port " + std::to_string(_port));
	if (this->port_defined == false)
		throw std::runtime_error("No port number defined in server block");
}

// ---------------------------------- SETTERS -----------------------------------------


void	Config::set_port(unsigned int &port) 								{_port = port;}
void	Config::set_max_size(size_t &max_size) 								{_max_size = max_size;}
void	Config::set_server_names(std::vector<std::string> &server_names)	{_server_names = server_names;}
void	Config::set_root(std::string &root) 								{_root = root;}
void	Config::set_cgi(std::string &cgi) 									{_cgi = cgi;}
void	Config::set_locations(const std::string &key, const Location &value){_locations[key] = value;}
void	Config::set_error_page(const int &key, const std::string &value)
{
	if (_error_pages.count(key))
		_error_pages[key] = value;
	else
		logger << Logger::warn << "No error code: " << key << std::endl;
}



// ---------------------------------- GETTERS -------------------------------------------


const unsigned int							&Config::get_port() const 		{return (_port);}
const size_t								&Config::get_max_size() const 	{return (_max_size);}
const std::vector<std::string>				&Config::get_server_names() const {return (_server_names);}
const std::string							&Config::get_root() const 		{return (_root);}
const std::string							&Config::get_cgi() const 		{return (_cgi);}
const Location								&Config::get_location(const std::string& key) const
{
	std::string longest_match;
	for (std::map<std::string, Location>::const_iterator it = this->_locations.begin(); it != this->_locations.end(); it++) {
		std::string location_path = it->first;
		if (location_path == key)
			return it->second;
		std::string longest = longest_match;
		if (location_path.back() != '/')
			location_path += '/';
		if (longest.back() != '/')
			longest += '/';
		if (location_path.length() < longest.length())
			continue ;
		if (key.substr(0, location_path.length()) == location_path)
			longest_match = it->first;
	}
	if (longest_match == "") {
		throw std::runtime_error("No location matched");
	}
	return this->_locations.at(longest_match);
}

const std::map<std::string,Location>	&Config::get_locations() const {
	return this->_locations;
}

const std::string							&Config::get_error_page(const int &key) const
{
	std::map<int,std::string>::const_iterator it = _error_pages.find(key);
	if (it == _error_pages.end())
		logger << Logger::warn << "Error code '" << key << "' not found" << std::endl;
	return (it->second);
}

// -------------------------------- OTHER FUNCTIONS -------------------------------------


void	Config::call_keyword_function(Config::conf_parser &enum_value, std::string &line)
{
	unsigned	int value;
	size_t		max_size;

	if (enum_value == LISTEN){
		if (this->port_defined == true)
			throw std::runtime_error("You can only declare the port number once");
		value = value_to_unsigned(line, false);
		if (value < 0 || value > 65535)
			throw std::runtime_error("Invalid port number. Min port number = 0, max port number = 65535");
		this->set_port(value);
		this->port_defined = true;
	}
	else if (enum_value == MAX_SIZE){
		value = value_to_unsigned(line, true);
		max_size = (size_t)value;
		this->set_max_size(max_size);
	}
	else if (enum_value == ROOT || enum_value == CGI)
		value_to_string(*this, line, enum_value);
	else if (enum_value == SERVER_NAME)
		value_to_string_vector(*this, line);
	else if (enum_value == ERROR_PAGE)
		value_to_error(*this, line);
}

//prints out a Config class
void	Config::print_config_class(void)
{
	logger << Logger::info << "address : " << this->get_port() << std::endl << std::endl;

	std::vector<std::string> server_names = this->get_server_names();
	logger << Logger::info << "server name(s) :" << std::endl;
	for (size_t i = 0; i < server_names.size(); i++)
		logger << Logger::info << server_names[i] << std::endl;
	logger << Logger::info << std::endl;
	logger << Logger::info << "root : " << this->get_root() << std::endl << std::endl;
	logger << Logger::info << "max_size : " << this->get_max_size() << std::endl << std::endl;
	logger << Logger::info << "cgi : " << this->get_cgi() << std::endl << std::endl;
	for (std::map<std::string, Location>::const_iterator it = _locations.begin(); it != _locations.end(); it++)
	{
		logger << Logger::info << "Location : " << it->first << std::endl;
		Location object = it->second;
		object.print_location_class();
		logger << Logger::info << std::endl;
	}
	logger << Logger::info << "\nError page :" << std::endl;
	for (std::map<int, std::string>::const_iterator it = _error_pages.begin(); it != _error_pages.end(); it++)
		logger << Logger::info << it->first << " : " << it->second << std::endl;
	logger << Logger::info << std::endl;
}


//Checks if the word found is one of the keywords
Config::conf_parser	determine_if_keyword(const std::string &word)
{
	if (word == "listen")
		return (Config::LISTEN);
	else if (word == "server_name")
		return (Config::SERVER_NAME);
	else if (word == "root")
		return (Config::ROOT);
	else if (word == "location")
		return (Config::LOCATION);
	else if (word == "client_max_body_size")
		return (Config::MAX_SIZE);
	else if (word == "error_page")
		return (Config::ERROR_PAGE);
	else if (word == "cgi")
		return (Config::CGI);
	return (Config::NOT_A_KEYWORD);
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
//if input is not only digits exception is trown
unsigned int	string_to_unsigned(std::string &word)
{
	if (word.empty())
		throw std::runtime_error("No integer value where int is expected in config file");
	for (size_t i = 0; i < word.length(); i++)
	{
		if (!isdigit(word[i]))
			throw std::runtime_error("No integer value where int is expected in config file");
	}
	return (std::stoul(word));
}

//If a second word is found it is converted to an unsigned int (if possible)
//If yes: the unsigned is set either to the 'port' value or 'max_body_size'
//if maxBody is true, k & K for kilo and m & M for mega
//For the maxBody : 2m or 2M gets converted to 2000000 (Mega byte)
//For the maxBody : 2k or 2K gets converted to 2000    (kilo byte)
unsigned int	value_to_unsigned(std::string &line, bool maxBody)
{
	bool	Mega = false;
	bool	kilo = false;
	std::string		word = get_second_word(line);

	if (!word.empty() && maxBody == true){
		if (word.back() == 'M' || word.back() == 'm'){
			Mega = true;
			word.pop_back();
		}
		else if (word.back() == 'K' || word.back() == 'k'){
			kilo = true;
			word.pop_back();
		}
	}
	unsigned int	value = string_to_unsigned(word);
	if (maxBody == true){
		if (Mega == true)
			value *= 1000000;
		else if (kilo == true)
			value *= 1000;
	}
	return (value);
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
void	value_to_string(Config &object, std::string &line, Config::conf_parser &enum_value)
{
	std::string	value = get_second_word(line);
	if (!value.empty())
	{
		if (enum_value == Config::ROOT)
			object.set_root(value);
		else if (enum_value == Config::CGI)
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
	if (error_code_string.empty())
		throw std::runtime_error("No error code provided in config file");
	error_code = stoi(error_code_string);
	if (error_code == 0)
		throw std::runtime_error("No correct error code provided in config file");
	value = get_third_word(line);
	if (value.empty())
		throw std::runtime_error("No error page provided for error code '" + std::to_string(error_code) + "' in config file");
	object.set_error_page(error_code, value);
}


//Creates a map<string,bool> for the methods map in the Location class
//All the values are iniated to be true at first
std::map<std::string, bool>	return_methods_map(void)
{
	std::map<std::string, bool>	methods;
	methods["GET"] = true;
	methods["POST"] = true;
	methods["DELETE"] = true;
	methods["PUT"] = false;
	methods["PATCH"] = false;
	return (methods);
}

void	set_methods_map_false(std::map<std::string, bool>& methods) {
    for (std::map<std::string, bool>::iterator it = methods.begin(); it != methods.end(); ++it) {
        it->second = false;
    }
}

//Creates a map<int,string> where all error codes are set to default path
std::map<int,std::string>	return_default_error_map(void)
{
	std::map<int,std::string> error;
	error[400] = "error_pages/400.html";
	error[403] = "error_pages/403.html";
	error[404] = "error_pages/404.html";
	error[405] = "error_pages/405.html";
	error[408] = "error_pages/408.html";
	error[413] = "error_pages/413.html";
	error[500] = "error_pages/500.html";
	error[501] = "error_pages/501.html";
	error[505] = "error_pages/505.html";
	return (error);
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
