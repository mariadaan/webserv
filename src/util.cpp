#include "util.hpp"

namespace util {
	std::vector<std::string> split_string(std::string str, std::string delim) {
		std::vector<std::string> result;

		size_t pos = 0;
		while (true) {
			size_t found = str.find(delim, pos);

			if (found == std::string::npos) {
				result.push_back(str.substr(pos));
				return result;
			}
			result.push_back(str.substr(pos, found - pos));
			pos = found + delim.length();
		}
	}

	unsigned char char_to_lower(char c) {
		return std::tolower(c);
	}

	void str_to_lower(std::string &str) {
		std::transform(str.begin(), str.end(), str.begin(), &char_to_lower);
	}
}
