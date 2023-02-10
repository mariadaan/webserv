#include "ParsedRequest.hpp"
#include "Logger.hpp"
#include "util.hpp"
#include <sstream>
#include <algorithm>
#include <cctype>

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

ParsedRequest::ParsedRequest(std::string str) : _headers_done(false) {
	this->parse_part(str);
}

/* Save location in ParsedRequest if the path is a location block.
	else, do nothing and do not initialise location. */
void ParsedRequest::set_location(std::map<std::string,Location> const &locations)
{
	if (method == GET) // allow GET by default
		this->is_allowed_method = true;
	else
		this->is_allowed_method = false;

	if (locations.count(this->path) > 0) {
		this->location = locations.at(this->path);
		this->location.print_location_class();
		this->is_allowed_method = this->location.get_request_methods(this->method_string);
	}
}

bool ParsedRequest::has_header(std::string key) const {
	util::str_to_lower(key);
	return (this->headers.count(key) > 0);
}

std::string const &ParsedRequest::get_header(std::string key) const {
	util::str_to_lower(key);
	return this->headers.at(key);
}

std::string ParsedRequest::get_query_string() const {
	std::string query;
	size_t found = this->path.find("?");
	if (found == std::string::npos) {
		return "";
	}
	return this->path.substr(found + 1);
}

std::string ParsedRequest::get_script_name() const {
	std::string script_name;
	size_t found = this->path.find("/cgi-bin/");
	if (found == std::string::npos) {
		return "";
	}
	script_name = this->path.substr(found);
	script_name = script_name.substr(0, script_name.find(".py") + 3);
	return script_name;
}

std::string ParsedRequest::get_auth_scheme() const {
	if (this->headers.count("Authorization") == 0) {
		return "";
	}
	std::string auth_header = this->headers.at("authorization");
	size_t found = auth_header.find(' ');
	if (found == std::string::npos) {
		throw std::runtime_error("Invalid Authorization header");
	}
	return auth_header.substr(0, found);
}

size_t ParsedRequest::get_content_length() const {
	if (this->headers.count("content-length") == 0) {
		throw std::runtime_error("No content-length header found");
	}
	std::string content_length_str = this->headers.at("content-length");
	std::stringstream ss(content_length_str);
	size_t content_length;
	ss >> content_length;
	if (ss.fail() || ss.bad() || !ss.eof()) {
		throw std::runtime_error("Invalid content-length header");
	}
	return content_length;
}

bool ParsedRequest::headers_finished() const {
	return this->_headers_done;
}

void ParsedRequest::parse_part(std::string part) {
	std::string header_delim("\r\n\r\n");
	size_t a = part.find(header_delim);
	this->_metadata += part.substr(0, a);
	if (part.size() < header_delim.size()) {
		size_t b = this->_metadata.find(header_delim, this->_metadata.size() - (header_delim.size() + part.size() - 1));
		if (b != std::string::npos) {
			this->body = this->_metadata.substr(b + header_delim.size());
			this->_metadata = this->_metadata.substr(0, b);
			this->_headers_done = true;
			this->_parse_metadata();
		}
	}
	else if (a != std::string::npos) {
		this->body = part.substr(a + header_delim.size()); // TODO: check if there is more to do
		this->_headers_done = true;
		this->_parse_metadata();
	}

}

bool ParsedRequest::_is_chunked(void) const {
	if (this->headers.count("transfer-encoding") == 0)
		return false;
	std::string transfer_encoding = this->headers.at("transfer-encoding");
	std::vector<std::string> encodings = util::split_string(transfer_encoding, ",");
	for (decltype(encodings)::const_iterator it = encodings.cbegin(); it != encodings.cend(); ++it) {
		std::string encoding = *it;
		size_t start = encoding.find_first_not_of(' ');
		if (start != std::string::npos)
			encoding = encoding.substr(start);
		if (encoding == "chunked")
			return true;
	}
	return false;
}

void ParsedRequest::_parse_metadata() {
	std::vector<std::string> lines = util::split_string(this->_metadata, "\r\n");
	std::vector<std::string> first_line_parts = util::split_string(lines[0], " ");
	if (first_line_parts.size() != 3) {
		throw std::runtime_error("Invalid first line");
	}
	this->method = ParsedRequest::_parse_method(first_line_parts[0]);
	this->method_string = first_line_parts[0];
	this->path = first_line_parts[1];
	this->http_version = first_line_parts[2];
	lines.erase(lines.begin());
	this->headers = ParsedRequest::_parse_headers(lines);
	this->is_chunked = this->_is_chunked();
}

Method ParsedRequest::_parse_method(std::string method_str) {
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

std::map<std::string, std::string> ParsedRequest::_parse_headers(std::vector<std::string> const lines) {
	std::map<std::string, std::string> headers;
	std::string delimiter(": "); // TODO: check if this space is always correct
	size_t delim_length = delimiter.length();
	for (decltype(lines)::const_iterator it = lines.cbegin(); it != lines.cend(); ++it) {
		std::string line = *it;
		if (line == "\r\n\r\n" || line.empty())
			return headers;
		size_t delim_pos = line.find(delimiter);
		if (delim_pos == std::string::npos) {
			throw std::runtime_error("header delim not found");
		}
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
	os << "Version: " << req.http_version << std::endl;
	os << "Headers: " << std::endl;
	for (decltype(req.headers)::const_iterator it = req.headers.cbegin(); it != req.headers.cend(); ++it) {
		os << "\t" << it->first << ": " << it->second << std::endl;
	}
	if (req.is_chunked)
		os << "Body is chunked, not stored here" << std::endl;
	else {
		os << std::endl;
		os << "Body: -----|" << req.body << "|-----" << std::endl;
	}
	return os;
}
