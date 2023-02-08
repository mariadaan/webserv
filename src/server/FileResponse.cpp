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
			this->_content_type = get_content_type(file_extension);
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
	return "HTTP/1.1 " + this->_response_status + CRLF + "Content-Type: " + this->_content_type + CRLF + CRLF + this->_page_content;
}

// if file_extension not in map, exception will be thrown
std::string get_content_type(std::string file_extension) {
	std::map<std::string, std::string> file_extension_to_content_type;
	file_extension_to_content_type["pdf"] = "application/pdf";
	file_extension_to_content_type["jpg"] = "image/jpeg";
	file_extension_to_content_type["jpeg"] = "image/jpeg";
	file_extension_to_content_type["ico"] = "image/x-icon";
	file_extension_to_content_type["png"] = "image/png";
	file_extension_to_content_type["gif"] = "image/gif";
	file_extension_to_content_type["mp3"] = "audio/mpeg";
	file_extension_to_content_type["mp4"] = "video/mp4";
	file_extension_to_content_type["html"] = "text/html";
	file_extension_to_content_type["css"] = "text/css";
	file_extension_to_content_type["js"] = "application/javascript";
	file_extension_to_content_type["json"] = "application/json";
	file_extension_to_content_type["xml"] = "application/xml";
	file_extension_to_content_type["zip"] = "application/zip";
	file_extension_to_content_type["gzip"] = "application/gzip";
	file_extension_to_content_type["bzip2"] = "application/x-bzip2";
	file_extension_to_content_type["tar"] = "application/x-tar";
	file_extension_to_content_type["rar"] = "application/x-rar-compressed";
	file_extension_to_content_type["txt"] = "text/plain";
	file_extension_to_content_type["csv"] = "text/csv";
	file_extension_to_content_type["xls"] = "application/vnd.ms-excel";
	file_extension_to_content_type["xlsx"] = "application/vnd.openxmlformats-officedocument.spreadsheetml.sheet";
	file_extension_to_content_type["doc"] = "application/msword";
	file_extension_to_content_type["docx"] = "application/vnd.openxmlformats-officedocument.wordprocessingml.document";
	file_extension_to_content_type["ppt"] = "application/vnd.ms-powerpoint";
	file_extension_to_content_type["pptx"] = "application/vnd.openxmlformats-officedocument.presentationml.presentation";
	file_extension_to_content_type["mpeg"] = "video/mpeg";
	file_extension_to_content_type["mpg"] = "video/mpeg";
	file_extension_to_content_type["mpe"] = "video/mpeg";
	file_extension_to_content_type["avi"] = "video/x-msvideo";
	file_extension_to_content_type["mov"] = "video/quicktime";
	file_extension_to_content_type["qt"] = "video/quicktime";
	file_extension_to_content_type["ogg"] = "audio/ogg";
	file_extension_to_content_type["oga"] = "audio/ogg";
	file_extension_to_content_type["flac"] = "audio/x-flac";
	file_extension_to_content_type["aac"] = "audio/aac";
	file_extension_to_content_type["wma"] = "audio/x-ms-wma";
	file_extension_to_content_type["wav"] = "audio/x-wav";
	file_extension_to_content_type["midi"] = "audio/midi";
	return file_extension_to_content_type.at(file_extension);
}
