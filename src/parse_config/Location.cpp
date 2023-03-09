#include "Location.hpp"
#include "Config.hpp"
#include "Logger.hpp"
// #include "configParser.hpp"

//                       **********************************************
//                       ************** LOCATION CLASS ****************
//                       **********************************************


// ------------------------------------- CONSTRUCTORS ----------------------------------------


Location::~Location(){}

Location::Location()
	: _request_methods(return_methods_map()), _auto_index(false)
{
}

Location::Location(std::vector<std::string> &location_body)
	: _request_methods(return_methods_map()), _max_size(0), _auto_index(false)
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
		else if (word == "upload" || word == "index" || word == "root"){
			std::string second = get_second_word(location_body[i]);
			if (second.empty())
				continue ;
			if (word == "upload")
				_upload = second;
			else if (word == "index")
				_index = second;
			else if (word == "root")
				_root = second;
		}
		else if (word == "request_method"){
			bool 				map_turned_to_false = false;
			std::stringstream	ss(location_body[i]);
			std::string			name;
			ss >> name;
			while (ss >> name){
				if (_request_methods.count(name)){
					if (map_turned_to_false == false){
						set_methods_map_false(_request_methods);
						map_turned_to_false = true;
					}
					_request_methods[name] = true;
				}
			}
		}
		else if (word == "client_max_body_size"){
			std::string	second = get_second_word(location_body[i]);
			if (second.empty())
				continue ;
			_max_size = (size_t)value_to_unsigned(location_body[i], true);
		}
		else if (word == "redirect"){
			std::string	second = get_second_word(location_body[i]);
			if (second.empty() || !this->_redirect.empty())
				continue ;
			this->_redirect = std::string(second);
		}
	}
}

// ----------------------------------------- GETTERS ----------------------------------------------

const std::string					&Location::get_index() const 			{return (_index);}
const std::string					&Location::get_upload() const 			{return (_upload);}
const std::string					&Location::get_root() const 			{return (_root);}
const std::string					&Location::get_redirect() const 		{return (_redirect);}
size_t								Location::get_max_size() const 			{return (_max_size);}
bool							 	Location::get_auto_index() const 		{return (_auto_index);}
bool								Location::get_request_methods(const std::string &key) const
{
	if (_request_methods.count(key)) // returns 1 if the key is found otherwise 0 is returned
		return (_request_methods.at(key)); //if the key is found the value at that key is returned
	return (false);
}

// -------------------------------------- OTHER FUNCTIONS -----------------------------------------

void	Location::set_max_size(size_t size){
	_max_size = size;
}

//prints out a Location class
void	Location::print_location_class(void)
{
	logger << Logger::info << "index : " << this->get_index() << std::endl;
	if (this->get_auto_index())
		logger << Logger::info << "autoindex : on" << std::endl;
	else
		logger << Logger::info << "autoindex : off" << std::endl;
	for (std::map<std::string,bool>::const_iterator it = _request_methods.begin(); it != _request_methods.end(); it++)
		logger << Logger::info << it->first << " : " << it->second << std::endl;
	logger << Logger::info << "upload : " << this->get_upload() << std::endl;
	logger << Logger::info << "root : " << _root << std::endl;
	logger << Logger::info << "maxBody : " << std::to_string(_max_size) << std::endl << std::endl;
}
