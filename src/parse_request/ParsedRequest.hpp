#pragma once
#ifndef PARSEDREQUEST_HPP
# define PARSEDREQUEST_HPP

#include "Config.hpp"
#include "Location.hpp"
#include "Optional.hpp"
#include "enums.hpp"
#include <vector>
#include <map>
#include <string>
#include <iostream>

std::ostream &operator<<(std::ostream &os, const Method &req);

class ParsedRequest {
public:
	Method 								method;
	std::string 						method_string;
	std::string 						path;
	std::string 						http_version;
	std::map<std::string, std::string> 	headers;
	std::string 						body;
	bool 								is_chunked;
	Optional<Location>					location;
	bool								is_allowed_method;

	ParsedRequest(Config &config, std::string str);
	std::string const	&get_header(std::string key) const;
	bool 				has_header(std::string key) const;
	std::string 		get_query_string() const;
	std::string 		get_script_name() const;
	std::string 		get_auth_scheme() const;
	size_t 				get_content_length() const;
	bool 				headers_finished() const;
	void 				parse_part(std::string part);

protected:
	ParsedRequest() {};

private:
	Config	*_config;
	static Method _parse_method(std::string method_str);
	static std::map<std::string, std::string> _parse_headers(std::vector<std::string> lines);
	bool _is_chunked(void) const;
	bool _headers_done;
	std::string _metadata;
	void _parse_metadata();
};

std::ostream &operator<<(std::ostream &os, const ParsedRequest &req);

#endif
