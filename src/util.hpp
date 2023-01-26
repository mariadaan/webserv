#ifndef UTIL_HPP
# define UTIL_HPP

#include <string>
#include <vector>

namespace util {
	std::vector<std::string> split_string(std::string str, std::string delim);
	unsigned char char_to_lower(char c);
	void str_to_lower(std::string &str);
}

#endif
