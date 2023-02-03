#ifndef CONFIG_HPP
# define CONFIG_HPP

#include "Location.hpp"

#include <iostream>
#include <vector>
#include <map>
#include <sstream> 


enum conf_parser {
	LISTEN = 1,
	SERVER_NAME,
	ROOT,
	LOCATION,
	MAX_SIZE,
	ERROR_PAGE,
	CGI
};

class Config
{
	private:
		unsigned int						_port;
		unsigned int						_max_size;
		std::vector<std::string>			_server_names;
		std::string							_root;
		std::string							_cgi;
		std::map<std::string,Location>		_locations;
		std::map<int, std::string>			_error_page;
	
	public:
		//constructors
		Config();
		~Config();
		Config& operator=(Config other); //copy and swap idiom 	(not needed yet)
		Config(std::vector<std::string> &server_vector);

		//getters
		const unsigned int				&get_port() const;
		const unsigned int				&get_max_size() const;
		const std::vector<std::string>	&get_server_names() const;
		const std::string				&get_root() const;
		const std::string				&get_cgi() const;
		const Location					&get_location(const std::string& key) const;
		const std::string				&get_error_page(const int &key) const;	

		//setters
		void	set_port(unsigned int &port);
		void	set_max_size(unsigned int &max_size);
		void	set_server_names(std::vector<std::string> &server_names);
		void	set_root(std::string &root);
		void	set_cgi(std::string &cgi);
		void	set_locations(const std::string &key, const Location &value);
		void	set_error_page(const int &key, const std::string &value);

		void	call_keyword_function(size_t &enumValue, std::string &line);
		void	print_config_class();
};

size_t									determine_if_keyword(const std::string &word);
std::vector<std::string>				return_location_body(std::vector<std::string> &server_vector, size_t i, size_t end);
unsigned int							string_to_unsigned(std::string &word);
void									value_to_unsigned(Config &object, std::string &line, size_t &enum_value);
void									value_to_string_vector(Config &object, std::string &line);
void									value_to_string(Config &object, std::string &line, size_t &enum_value);
void									value_to_error(Config &object, std::string &line);
std::map<std::string, bool>				return_false_methods_map(void);
std::map<int,std::string>				return_default_error_map(void);
std::string								get_third_word(std::string &line);
std::string								get_second_word(std::string &line);
std::string 							find_first_word(std::string &line) ;

#endif