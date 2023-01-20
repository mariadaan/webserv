#ifndef PARSEDREQUEST_HPP
# define PARSEDREQUEST_HPP

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
	std::string method_string;
	std::string path;
	std::string http_version;
	std::map<std::string, std::string> headers;
	std::string body;
	bool is_chunked;

	ParsedRequest(std::string str);
	std::string const &get_header(std::string key) const;
	std::string get_query_string() const;
	std::string get_auth_scheme() const;

protected:
	ParsedRequest() {};

private:
	static Method _parse_method(std::string method_str);
	static std::map<std::string, std::string> _parse_headers(std::vector<std::string> lines);
	bool _is_chunked(void) const;
};

std::ostream &operator<<(std::ostream &os, const ParsedRequest &req);

#endif
