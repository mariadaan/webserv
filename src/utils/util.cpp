#include "util.hpp"

namespace util {
	std::vector<std::string> split_string(std::string const &str, std::string const &delim) {
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

	bool can_open_file(const std::string& filename) {
		std::ifstream file(filename);
		if (file.is_open()) {
			file.close();
			return true;
		}
		else {
			return false;
		}
	}

	std::string file_to_str(std::string const &filename) {
		std::ifstream file(filename);
		std::string str;
		if(file) {
			std::ostringstream ss;
			ss << file.rdbuf();
			str = ss.str();
		}
		return str;
	}

	
}
