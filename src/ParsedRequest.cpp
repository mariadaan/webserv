#include "ParsedRequest.hpp"
#include <algorithm>
#include <cctype>

namespace util {
	static std::vector<std::string> split_string(std::string str, std::string delim) {
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

	static unsigned char char_to_lower(char c) {
		return std::tolower(c);
	}

	static void str_to_lower(std::string &str) {
		std::transform(str.begin(), str.end(), str.begin(), &char_to_lower);
	}
}

std::ostream &operator<<(std::ostream &os, const Method &method) {
	static std::map<Method, std::string> methods;
	methods[GET] = "GET";
	methods[POST] = "POST";
	methods[PUT] = "PUT";
	methods[DELETE] = "DELETE";
	methods[HEAD] = "HEAD";
	methods[CONNECT] = "CONNECT";
	methods[OPTIONS] = "OPTIONS";
	methods[TRACE] = "TRACE";
	methods[PATCH] = "PATCH";
	methods[UNKNOWN] = "??? UNKNOWN ???";
	os << methods[method];
	return os;
}

ParsedRequest::ParsedRequest(std::string str) {
	std::string metadata;
	size_t found = str.find("\r\n\r\n");
	if (found == std::string::npos) {
		this->body = "";
		metadata = str;
	}
	else {
		this->body = str.substr(found + 4);
		metadata = str.substr(0, found);
	}
	std::vector<std::string> lines = util::split_string(metadata, "\r\n");
	std::vector<std::string> first_line_parts = util::split_string(lines[0], " ");
	if (first_line_parts.size() != 3) {
		throw std::runtime_error("Invalid first line");
	}
	this->method = ParsedRequest::parse_method(first_line_parts[0]);
	this->path = first_line_parts[1];
	this->version = first_line_parts[2];
	lines.erase(lines.begin());
	this->headers = ParsedRequest::parse_headers(lines);
}

Method ParsedRequest::parse_method(std::string method_str) {
	static std::map<std::string, Method> methods;
	methods["GET"] = GET;
	methods["POST"] = POST;
	methods["PUT"] = PUT;
	methods["DELETE"] = DELETE;
	methods["HEAD"] = HEAD;
	methods["CONNECT"] = CONNECT;
	methods["OPTIONS"] = OPTIONS;
	methods["TRACE"] = TRACE;
	methods["PATCH"] = PATCH;
	if (methods.count(method_str) == 0)
		throw std::runtime_error("Invalid method");
	return methods[method_str];
}

std::map<std::string, std::string> ParsedRequest::parse_headers(std::vector<std::string> const lines) {
	std::map<std::string, std::string> headers;
	std::string delimiter(": ");
	size_t delim_length = delimiter.length();
	for (decltype(lines)::const_iterator it = lines.cbegin(); it != lines.cend(); ++it) {
		std::string line = *it;
		size_t delim_pos = line.find(delimiter);
		std::string key = line.substr(0, delim_pos);
		util::str_to_lower(key);
		std::string value = line.substr(delim_pos + delim_length);
		headers[key] = value;
	}
	return headers;
}

std::ostream &operator<<(std::ostream &os, const ParsedRequest &req) {
	os << "Method: " << req.method << std::endl;
	os << "Path: " << req.path << std::endl;
	os << "Version: " << req.version << std::endl;
	os << "Headers: " << std::endl;
	for (decltype(req.headers)::const_iterator it = req.headers.cbegin(); it != req.headers.cend(); ++it) {
		os << "\t" << it->first << ": " << it->second << std::endl;
	}
	os << std::endl;
	os << "Body: " << std::endl;
	os << req.body << std::endl;
	return os;
}