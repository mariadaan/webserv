#pragma once
#ifndef PARSEDCONFIGFILE_HPP
# define PARSEDCONFIGFILE_HPP

#include "Config.hpp"

#include <fstream>
#include <iostream>
#include <vector>
#include <sstream>

class ParsedConfigFile {
public:
	ParsedConfigFile(char const *conf_file);

	void	parse_config(char const *conf_file);

	std::vector<Config>		server_blocks;

};

std::vector<Config>	set_config_vector(std::vector<std::vector<std::string> > &server_vector);
const std::vector<std::string>	file_to_lines(const std::string &file_name);
std::vector<std::vector<std::string> >	create_server_vector(const std::vector<std::string> &file);
std::vector<std::string>	find_server_block(const std::vector<std::string> &vec);

bool			check_brackets(const std::vector<std::string> &vec);
std::string		truncate_string(const std::string &str, char c);

/* general config utils */
size_t			find_closing_bracket(const std::vector<std::string> &vec, size_t line);

#endif
