#ifndef FILERESPONSE_HPP
# define FILERESPONSE_HPP

#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <map>

#define CLRF "\r\n"

enum HTTP_STATUS_CODES {
	HTTP_OK = 200,
	HTTP_CREATED = 201,
	HTTP_NO_CONTENT = 204,
	HTTP_BAD_REQUEST = 400,
	HTTP_UNAUTHORIZED = 401,
	HTTP_FORBIDDEN = 403,
	HTTP_NOT_FOUND = 404,
	HTTP_INTERNAL_SERVER_ERROR = 500,
	HTTP_SERVICE_UNAVAILABLE = 503
};

class FileResponse {
public:
	FileResponse(const std::string &request_path);

	bool			can_open_file();
	void			load_content();
	void			generate_response();
	std::string		get_response() const;
	std::string		get_response_status(int status_code) const;

private:
	std::string		_file_dir;
	std::string		_filename;
	bool			_file_accessible;
	bool			_is_home;
	std::string		_page_content;
	std::string		_response_status;
	std::string		_content_type;
};

std::string get_content_type(std::string file_extension);

#endif
