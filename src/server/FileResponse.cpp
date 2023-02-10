#include "FileResponse.hpp"
#include "Logger.hpp"
#include "util.hpp"

FileResponse::FileResponse(Config &config, ParsedRequest &request) : config(config), request(request) {
	this->_file_dir = config.get_root() + "/";
	this->_filename = this->_file_dir + this->request.path;
	if (this->request.location.is_set())
		this->_filename = this->_file_dir + this->request.location.get_index();
	else
		this->_filename = this->_file_dir + this->request.path;
	this->generate_response();
	logger << Logger::debug << "filename: " << this->_filename << std::endl;
}

bool FileResponse::can_open_file() {
	std::ifstream file(this->_filename);
	if (file.is_open()) {
		file.close();
		return true;
	}
	else {
		logger << Logger::debug << "Could not open " << this->_filename << std::endl;
		return false;
	}
}

void FileResponse::load_page_content(void) {
	this->_file_accessible = this->can_open_file();
	if (this->_file_accessible) {
		this->_page_content = util::file_to_str(this->_filename);
	}
}

void FileResponse::define_status(void) {
	if (!this->_file_accessible) {
		this->_status_code = HTTP_NOT_FOUND;
	}
	else if (this->request.is_allowed_method == false) {
		this->_status_code = HTTP_METHOD_NOT_ALLOWED;
	}
	else if (this->request.body.empty() && this->_page_content.empty()) {
		this->_status_code = HTTP_NO_CONTENT;
	}
	else if (this->request.method == POST) {
		this->_status_code = HTTP_CREATED;
	}
	else {
		this->_status_code = HTTP_OK;
	}
}

void FileResponse::define_content_type(void) {
	std::string file_extension;

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
}

void FileResponse::generate_response(void) {
	this->load_page_content();
	this->define_status();
	this->_response_status = this->get_response_status(this->_status_code);
	
	if (this->_status_code >=400) {
		this->_filename = this->_file_dir + this->config.get_error_page(this->_status_code);
		this->load_page_content();
	}
	this->define_content_type();
}

std::string FileResponse::get_response(void) const {
	return this->request.http_version + " " + this->_response_status + CRLF + "Content-Type: " + this->_content_type + CRLF + CRLF + this->_page_content;
}

std::string FileResponse::get_response_status(int status_code) const
{
	std::map<int, std::string> code_to_header;
	code_to_header[HTTP_OK] = "200 OK";
	code_to_header[HTTP_CREATED] = "201 Created";
	code_to_header[HTTP_NO_CONTENT] = "204 No Content";
	// code_to_header[HTTP_BAD_REQUEST] = "400 Bad Request";
	// code_to_header[HTTP_UNAUTHORIZED] = "401 Unauthorized";
	// code_to_header[HTTP_FORBIDDEN] = "403 Forbidden";
	code_to_header[HTTP_NOT_FOUND] = "404 Not Found";
	code_to_header[HTTP_METHOD_NOT_ALLOWED] = "405 Method Not Allowed";
	// code_to_header[HTTP_INTERNAL_SERVER_ERROR] = "500 Internal Server Error";
	// code_to_header[HTTP_SERVICE_UNAVAILABLE] = "503 Service Unavailable";
	return code_to_header.at(status_code);
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
