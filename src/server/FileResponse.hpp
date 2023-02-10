#ifndef FILERESPONSE_HPP
# define FILERESPONSE_HPP

#include "Config.hpp"
#include "ParsedRequest.hpp"
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <map>

#define CRLF "\r\n"

enum HTTP_STATUS_CODES {
	HTTP_OK = 200,
	HTTP_CREATED = 201,
	HTTP_NO_CONTENT = 204,
	// HTTP_BAD_REQUEST = 400, // use this if we can't parse the request?
	// HTTP_UNAUTHORIZED = 401,
	// HTTP_FORBIDDEN = 403,
	HTTP_NOT_FOUND = 404,
	HTTP_METHOD_NOT_ALLOWED = 405
	// HTTP_INTERNAL_SERVER_ERROR = 500, // how could we ever use this one?
	// HTTP_SERVICE_UNAVAILABLE = 503
};

class FileResponse {
public:
	FileResponse(Config &config, ParsedRequest &request);

	Config			&config;
	ParsedRequest	&request;

	std::string		get_response() const;
	std::string		get_response_status(int status_code) const;

private:
	bool			can_open_file();
	void			load_page_content();
	void			define_status();
	void			define_content_type();
	void			generate_response();
	std::string		_file_dir;
	std::string		_filename;
	bool			_file_accessible;
	int				_status_code;
	std::string		_page_content;
	std::string		_response_status;
	std::string		_content_type;
};

std::string get_content_type(std::string file_extension);

#endif
