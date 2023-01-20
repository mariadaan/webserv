#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

/* utils */
std::string file_to_str(std::string filename) {
	std::ifstream f(filename); //taking file as inputstream
	std::string str;
	if(f) {
		std::ostringstream ss;
		ss << f.rdbuf(); // reading data
		str = ss.str();
	}
	return str;
}

void print(std::string message) {
	std::cout << message << std::endl;
}