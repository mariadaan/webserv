#ifndef UTIL_HPP
# define UTIL_HPP

#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <iostream>

namespace util {
	std::vector<std::string> split_string(std::string const &str, std::string const &delim);
	unsigned char char_to_lower(char c);
	void str_to_lower(std::string &str);
	std::string file_to_str(std::string const &filename);

	// ja het is een beetje lelijk maar je moet wat zonder github copilot he
	template <typename T>
	void print(T message) {
		std::cout << message << std::endl;
}
}

#endif
