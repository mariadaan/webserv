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
#include <dirent.h>

class FileResponse {
public:
	FileResponse(Config &config, ParsedRequest &request);

	Config			&config;
	ParsedRequest	&request;

	std::string		get_response() const;

private:
	bool				can_open_file();
	void				load_page_content();
	void				define_status();
	void				define_content_type();
	void				define_auto_index();
	void				generate_response();
	void				directory_listing();
	void				delete_files(std::string path);
	std::string			_file_dir;
	std::string			_filename;
	bool				_file_accessible;
	bool				_auto_indexing;
	HTTP_STATUS_CODES	_status_code;
	std::string			_page_content;
	std::string			_response_status;
	std::string			_content_type;
};

#endif
