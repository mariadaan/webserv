#ifndef CONFIGPARSER_HPP
# define CONFIGPARSER_HPP

#include <stdlib.h>
#include <string>
#include <iostream>
#include <stdexcept>
#include <fstream>
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

class Location
{
	private:
		std::string					_index;
		std::map<std::string, bool>	_request_methods;
		std::string					_upload;
		bool						_auto_index;

	public:
		//constructors
		Location();
		Location(std::vector<std::string> &location_body);
		~Location();

		//getters
		const std::string					&get_index() const;
		const std::string					&get_upload() const;
		bool								get_request_methods(const std::string &key) const;
		bool							 	get_auto_index() const;

		void	print_location_class();
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


//print.cpp
void									print_string_vector(std::vector<std::string> &vec);
void									print_server_vector(std::vector<std::vector<std::string> > &vec);
//configUtils.cpp
size_t									determine_if_keyword(const std::string &word);
std::vector<std::string>				return_location_body(std::vector<std::string> &server_vector, size_t i, size_t end);
unsigned int							string_to_unsigned(std::string &word);
void									value_to_unsigned(Config &object, std::string &line, size_t &enum_value);
void									value_to_string_vector(Config &object, std::string &line);
void									value_to_string(Config &object, std::string &line, size_t &enum_value);
void									value_to_error(Config &object, std::string &line);
std::map<std::string, bool>				return_false_methods_map(void);
std::map<int,std::string>				return_default_error_map(void);
std::string								truncate_string(const std::string &str, char c);
std::string								get_third_word(std::string &line);
std::string								get_second_word(std::string &line);
std::string 							find_first_word(std::string &line) ;
bool									check_brackets(const std::vector<std::string> &vec);
size_t									find_closing_bracket(const std::vector<std::string> &vec, size_t line);
//configParser.cpp
std::vector<Config>						set_config_vector(std::vector<std::vector<std::string> > &server_vector);
std::vector<std::string>				find_server_block(const std::vector<std::string> &vec);
std::vector<std::vector<std::string> >	create_server_vector(const std::vector<std::string> &file);
const std::vector<std::string>			get_file_vector(const std::string &file_name);
//parse_config.cpp
std::vector<Config>						parse_config(char const *conf_file);

#endif
