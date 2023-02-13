#pragma once
#ifndef FILERESPONSE_HPP
# define FILERESPONSE_HPP

#include "Config.hpp"
#include "ParsedRequest.hpp"
#include "enums.hpp"
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <map>

class FileResponse {
public:
	FileResponse(Config &config, ParsedRequest &request);

	Config			&config;
	ParsedRequest	&request;

	std::string		get_response() const;
	std::string		get_response_status(int status_code) const;

private:
	bool				can_open_file();
	void				load_page_content();
	void				define_status();
	void				define_content_type();
	void				generate_response();
	std::string			_file_dir;
	std::string			_filename;
	bool				_file_accessible;
	HTTP_STATUS_CODES	_status_code;
	std::string			_page_content;
	std::string			_response_status;
	std::string			_content_type;
};

std::string get_content_type(std::string file_extension);

#endif
