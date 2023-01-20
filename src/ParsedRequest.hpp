#ifndef PARSEDREQUEST_HPP
# define PARSEDREQUEST_HPP

#include <vector>
#include <map>
#include <list>
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
	std::list<std::string> chunks; // ik denk dat we dit nodig hebben om de chunks in op te slaan
	std::string body;
	bool is_chunked;

	ParsedRequest(std::string str);
	std::string get_body(void) const;

protected:
	ParsedRequest() {};

private:
	static Method _parse_method(std::string method_str);
	static std::map<std::string, std::string> _parse_headers(std::vector<std::string> lines);
	bool _is_chunked(void) const;
	std::list<std::string>_parse_chunks(void); // marius je moet mij even uitleggen wanneer een functie nou static moet zijn
};

std::ostream &operator<<(std::ostream &os, const ParsedRequest &req);

#endif
