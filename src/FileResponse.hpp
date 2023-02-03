#ifndef FILERESPONSE_HPP
# define FILERESPONSE_HPP

#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <map>

class FileResponse {
public:
	FileResponse(const std::string &request_path);

	bool can_open_file();
	void load_content();
	void generate_response();
	std::string get_response() const;

private:
	std::string _file_dir;
	std::string _filename;
	bool _file_accessible;
	bool _is_home;
	std::string _page_content;
	std::string _response_headers;
	std::string _content_type;
};

#endif
