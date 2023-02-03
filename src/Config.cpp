#include "Config.hpp"
#include "configParser.hpp"

//                 **********************************************
//                 **************** CONFIG CLASS ****************
//                 **********************************************


// -------------------------------- CONSTRUCTORS ---------------------------------


Config::Config()
//Default constructor
{
	_port = 80;
	_max_size = 1;
	_error_page = return_default_error_map();
}

//No dynamically allocated memmory in the class so destructor is empty
Config::~Config(){}


//Copy assignent operator using the copy and swap idiom
//Note that the object 'other' is given as value instead of reference
//Therefore the object gets copied and it does not touch the original object
//The copy and swap idiom has multiple benifits (look up if interested)
Config& Config::operator=(Config other)
{
	std::swap(*this, other);
	return *this;
}

Config::Config(std::vector<std::string> &server_vector)
	: _port(80), _max_size(1), _error_page(return_default_error_map())
{
	size_t	enum_value;

	for (size_t i = 0; i < server_vector.size(); i++)
	{
		std::string word = find_first_word(server_vector[i]);
		enum_value = determine_if_keyword(word);
		if (enum_value == 0)
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
}


// ---------------------------------- SETTERS -----------------------------------------


void	Config::set_port(unsigned int &port) 								{_port = port;}
void	Config::set_max_size(unsigned int &max_size) 						{_max_size = max_size;}
void	Config::set_server_names(std::vector<std::string> &server_names)	{_server_names = server_names;}
void	Config::set_root(std::string &root) 								{_root = root;}
void	Config::set_cgi(std::string &cgi) 									{_cgi = cgi;}
void	Config::set_locations(const std::string &key, const Location &value){_locations[key] = value;}
void	Config::set_error_page(const int &key, const std::string &value)
{
	if (_error_page.count(key))
		_error_page[key] = value;
	else
		std::cerr << "No error code: " << key << std::endl; 			//THROW !
}



// ---------------------------------- GETTERS -------------------------------------------


const unsigned int							&Config::get_port() const 		{return (_port);}
const unsigned int							&Config::get_max_size() const 	{return (_max_size);}
const std::vector<std::string>				&Config::get_server_names() const {return (_server_names);}
const std::string							&Config::get_root() const 		{return (_root);}
const std::string							&Config::get_cgi() const 		{return (_cgi);}
const Location								&Config::get_location(const std::string& key) const 
{
	return (this->_locations.at(key));
	//  std::map<std::string,Location>::const_iterator it = _locations.find(key);
	// if (it == _locations.end())
	// 	std::cerr << "Location: '" << key << "' not found" << std::endl; 		//THROW!
	// 	// throw std::out_of_range("Location not found");
	// return (it->second);
}
const std::string							&Config::get_error_page(const int &key) const
{
	std::map<int,std::string>::const_iterator it = _error_page.find(key);
	if (it == _error_page.end())
		std::cerr << "Error code '" << key << "' not found" << std::endl;	 //THROW!
		//throw
	return (it->second);
}

// -------------------------------- OTHER FUNCTIONS -------------------------------------



void	Config::call_keyword_function(size_t &enum_value, std::string &line)
{

	if (enum_value == LISTEN || enum_value == MAX_SIZE)
		value_to_unsigned(*this, line, enum_value);
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
	std::cout << "address : " << this->get_port() << std::endl << std::endl;
	
	std::vector<std::string> server_names = this->get_server_names();
	std::cout << "server name(s) :" << std::endl;
	for (size_t i = 0; i < server_names.size(); i++)
		std::cout << server_names[i] << std::endl;
	std::cout << std::endl;
	std::cout << "root : " << this->get_root() << std::endl << std::endl;
	std::cout << "max_size : " << this->get_max_size() << std::endl << std::endl;
	std::cout << "cgi : " << this->get_cgi() << std::endl << std::endl;
	for (std::map<std::string, Location>::const_iterator it = _locations.begin(); it != _locations.end(); it++)
	{
		std::cout << "Location : " << it->first << std::endl;
		Location object = it->second;
		object.print_location_class();
		std::cout << std::endl;
	}
	std::cout << "\nError page :" << std::endl;
	for (std::map<int, std::string>::const_iterator it = _error_page.begin(); it != _error_page.end(); it++)
		std::cout << it->first << " : " << it->second << std::endl;
	std::cout << std::endl;
}
