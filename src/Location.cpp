#include "Location.hpp"
#include "configParser.hpp"

//                       **********************************************
//                       ************** LOCATION CLASS ****************
//                       **********************************************


// ------------------------------------- CONSTRUCTORS ----------------------------------------


Location::~Location(){}

Location::Location()
	: _request_methods(return_false_methods_map()), _auto_index(false)
{
}

Location::Location(std::vector<std::string> &location_body)
	: _request_methods(return_false_methods_map()), _auto_index(false)
{
	for (size_t i = 0; i < location_body.size(); i++)
	{
		std::string word = find_first_word(location_body[i]);
		if (word.empty())
			continue;
		else if (word == "autoindex"){
			std::string second = get_second_word(location_body[i]);
			if (second == "ON")
				_auto_index = true;
		}
		else if (word == "upload" || word == "index"){
			std::string second = get_second_word(location_body[i]);
			if (second.empty())
				continue ;
			if (word == "upload")
				_upload = second;
			else if (word == "index")
				_index = second;
		}
		else if (word == "request_method")
		{
			std::stringstream	ss(location_body[i]);
			std::string			name;
			ss >> name;
			while (ss >> name)
			{
				if (_request_methods.count(name))
					_request_methods[name] = true;
			}
		}
	}
}


// ----------------------------------------- GETTERS ----------------------------------------------

const std::string					&Location::get_index() const 			{return (_index);}
const std::string					&Location::get_upload() const 			{return (_upload);}
bool							 	Location::get_auto_index() const 		{return (_auto_index);}
bool								Location::get_request_methods(const std::string &key) const
{
	if (_request_methods.count(key)) // returns 1 if the key is found otherwise 0 is returned
		return (_request_methods.at(key)); //if the key is found the value at that key is returned
	return (false);
}

// -------------------------------------- OTHER FUNCTIONS -----------------------------------------

//prints out a Location class
void	Location::print_location_class(void)
{
	std::cout << "index : " << this->get_index() << std::endl;
	if (this->get_auto_index())
		std::cout << "autoindex : on" << std::endl;
	else
		std::cout << "autoindex : off" << std::endl;
	for (std::map<std::string,bool>::const_iterator it = _request_methods.begin(); it != _request_methods.end(); it++)
		std::cout << it->first << " : " << it->second << std::endl;
	std::cout << "upload : " << this->get_upload() << std::endl;
}