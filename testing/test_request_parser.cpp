#include <criterion/criterion.h>
#include "../src/ParsedRequest.hpp"

#include<iostream>
#include<fstream>
#include<sstream>
#include<string>


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

Test(ParsedRequest, ParsedRequest) {
	std::string request = file_to_str("request_examples/req1.txt");
	ParsedRequest parsedRequest(request);
	cr_assert_eq(parsedRequest.body, "Hello world", "Incorrect body");
}