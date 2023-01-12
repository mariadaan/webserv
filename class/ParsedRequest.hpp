#include <vector>
#include <map>
#include <string>
#include <iostream>

enum Method {
	GET,
	POST,
	PUT,
	DELETE,
	HEAD,
	CONNECT,
	OPTIONS,
	TRACE,
	PATCH,
	UNKNOWN
};
std::ostream &operator<<(std::ostream &os, const Method &req);

class ParsedRequest {
public:
	Method method;
	std::string path;
	std::string version;
	std::map<std::string, std::string> headers;
	std::string body;

	ParsedRequest(std::string str);

private:
	static Method	parse_method(std::string method_str);
	static std::map<std::string, std::string>	parse_headers(std::vector<std::string> lines);
};

std::ostream &operator<<(std::ostream &os, const ParsedRequest &req);
