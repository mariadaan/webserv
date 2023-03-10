#pragma once
#ifndef UTIL_HPP
# define UTIL_HPP

#include "enums.hpp"
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>

namespace util {
	std::vector<std::string> split_string(std::string const &str, std::string const &delim);
	unsigned char char_to_lower(char c);
	void str_to_lower(std::string &str);
	bool can_open_file(const std::string& filename);
	bool is_file(const char* filename);
	std::string file_to_str(std::string const &filename);
	void replace_all(std::string& str, const std::string& from, const std::string& to);
	std::string get_file_extension(std::string filename);
	std::string get_content_type(std::string file_extension);
	bool can_open_file(std::string const &filename);
	std::string get_response_status(HTTP_STATUS_CODES status_code);
}

#endif
