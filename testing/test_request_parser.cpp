#include <criterion/criterion.h>
#include "../src/ParsedRequest.hpp"
#include "test_utils.hpp"

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

// most basic request
Test(basic_01, ParsedRequest) {
	std::string request = file_to_str("request_examples/req01.txt");
	ParsedRequest parsedRequest(request);
	cr_assert_eq(parsedRequest.path, "/", "Incorrect path");
	cr_assert_eq(parsedRequest.version, "HTTP/1.1", "Incorrect version");
	cr_assert_eq(parsedRequest.body, "Hello world", "Incorrect body");
	cr_assert_eq(parsedRequest.is_chunked, false, "Incorrect is_chunked");
}

Test(chunked_01, ParsedRequest) {
	std::string request = file_to_str("request_examples/req02.txt");
	ParsedRequest parsedRequest(request);
	cr_assert_eq(parsedRequest.path, "/upload", "Incorrect path");
	cr_assert_eq(parsedRequest.version, "HTTP/1.1", "Incorrect version");
	cr_assert_eq(parsedRequest.body, "hello world!", "Incorrect body");
	cr_assert_eq(parsedRequest.is_chunked, true, "Incorrect is_chunked");
}