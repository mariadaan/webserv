#include "FileResponse.hpp"
#include "Logger.hpp"
#include "util.hpp"

FileResponse::FileResponse(const std::string &request_path){
	this->_file_dir = "./root/var/www"; // to be defined by config
	this->_filename = this->_file_dir + request_path;
	this->_file_accessible = this->can_open_file();
	if (request_path == "/") // TODO: dit verbeteren want dit is lelijk
		this->_is_home = true;
	else
		this->_is_home = false;
}

bool FileResponse::can_open_file() {
	std::ifstream file(this->_filename);
	if (file.is_open()) {
		file.close();
		return true;
	}
	else {
		return false;
	}
}

void FileResponse::load_content(void) {
	if (this->_file_accessible) {
		if (this->_is_home)
			this->_filename = this->_filename + "index.html";
		this->_page_content = util::file_to_str(this->_filename);
	}
	else {
		this->_page_content = util::file_to_str(this->_file_dir + "/notfound.html");
	}
}

void FileResponse::generate_response(void) {
	std::string file_extension;

	this->load_content();
	if (!this->_file_accessible) {
		this->_response_status = this->get_response_status(HTTP_NOT_FOUND);
	}
	else {
		if (this->_filename.find_last_of(".") == std::string::npos
			|| this->_filename.find_last_of(".") == 0) {
			file_extension = "txt";
		}
		else
			file_extension = this->_filename.substr(this->_filename.find_last_of(".") + 1);
		try
		{
			this->_content_type = util::get_content_type(file_extension);
		}
		catch(const std::exception& e)
		{
			logger << Logger::error << "Requested file type with extension \"" << file_extension << "\" invalid." << std::endl;
		}
		this->_response_status = this->get_response_status(HTTP_OK);
	}
}

std::string FileResponse::get_response_status(int status_code) const
{
	std::map<int, std::string> code_to_header;
	code_to_header[HTTP_OK] = "200 OK";
	code_to_header[HTTP_CREATED] = "201 Created";
	code_to_header[HTTP_NO_CONTENT] = "204 No Content";
	code_to_header[HTTP_BAD_REQUEST] = "400 Bad Request";
	code_to_header[HTTP_UNAUTHORIZED] = "401 Unauthorized";
	code_to_header[HTTP_FORBIDDEN] = "403 Forbidden";
	code_to_header[HTTP_NOT_FOUND] = "404 Not Found";
	code_to_header[HTTP_INTERNAL_SERVER_ERROR] = "500 Internal Server Error";
	code_to_header[HTTP_SERVICE_UNAVAILABLE] = "503 Service Unavailable";
	return code_to_header.at(status_code);
}

std::string FileResponse::get_response(void) const {
	return "HTTP/1.1 " + this->_response_status + CLRF + "Content-Type: " + this->_content_type + CLRF + CLRF + this->_page_content;
}
