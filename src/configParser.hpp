#ifndef CONFIGPARSER_HPP
# define CONFIGPARSER_HPP

#include "Config.hpp"
#include "Location.hpp"

#include <stdlib.h>
#include <string>
#include <iostream>
#include <stdexcept>
#include <fstream>
#include <iostream>
#include <vector>
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
