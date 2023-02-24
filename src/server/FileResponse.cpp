#include "FileResponse.hpp"
#include "Logger.hpp"
#include "util.hpp"
#include "defines.hpp"

FileResponse::FileResponse(Config &config, ParsedRequest &request) : config(config), request(request) {
	this->_file_dir = config.get_root() + "/";
	this->_filename = this->_file_dir + this->request.path;
	logger << Logger::debug << "Filename: " << this->_filename << std::endl;
	std::string index_file = this->_filename + "/" + this->request.location.get_index();
	logger << Logger::debug << "index file: " << index_file << std::endl;
	if (this->request.location.is_set() && !this->request.location.get_index().empty() && util::can_open_file(index_file)) {
		this->_filename = index_file;
	}
	// else if (this->request.location.is_set() && !this->request.location.get_redirect().empty()) {
	// 	this->_filename = this->_file_dir + this->request.location.get_redirect();
	// }
	else
		this->_filename = this->_file_dir + this->request.path;
	this->define_auto_index();
	this->generate_response();
	logger << Logger::debug << "Filename: " << this->_filename << std::endl;
}

void FileResponse::define_auto_index(void) {
	if (this->request.location.is_set() && this->request.location.get_auto_index() && (this->request.location.get_index().empty() || (!this->request.location.get_index().empty() && !util::can_open_file(this->_file_dir + this->request.location.get_index()))))
		this->_auto_indexing = true;
	else
		this->_auto_indexing = false;
}

bool FileResponse::can_open_file() {
	bool can_open = util::can_open_file(this->_filename);
	if (!can_open) {
		logger << Logger::warn << "Could not open " << this->_filename << std::endl;
	}
	return can_open;
}

void FileResponse::load_page_content(void) {
	this->_file_accessible = this->can_open_file();
	if (this->_file_accessible) {
		this->_page_content = util::file_to_str(this->_filename);
	}
}

void FileResponse::define_status(void) {
	if (this->request.location.is_set() && !this->request.location.get_redirect().empty())
	{
		this->_status_code = HTTP_MOVED_PERMANENTLY;
	}
	else if (!this->_file_accessible) {
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
	std::string file_extension = util::get_file_extension(this->_filename);

	try
	{
		this->_content_type = util::get_content_type(file_extension);
	}
	catch(const std::exception& e)
	{
		logger << Logger::error << "Requested file type with extension \"" << file_extension << "\" invalid." << std::endl;
	}
}

void FileResponse::directory_listing(void) {
	DIR* dir = opendir(this->_filename.c_str());
	if (dir == nullptr) {
		throw std::runtime_error("Error opening directory: " + this->_filename);
	}
	struct dirent* entry;
	while ((entry = readdir(dir)) != nullptr) {
		// Ignore . and .. directories
		if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
			continue;
		}
		std::string filename(entry->d_name);
		if (entry->d_type == DT_DIR) {
			this->_page_content += "<li><a href='" + filename + "/'>" + filename + "/ </a></li>" + CRLF;
		} else {
			this->_page_content += "<li><a href='" + filename + "'>" + filename + " </a></li>" + CRLF;
		}
	}
	closedir(dir);
}

void FileResponse::generate_response(void) {
	if (this->_auto_indexing) {
		this->_response_status = HTTP_OK;
		this->_content_type = "text/html";
		try {
			this->directory_listing();
			return ;
		}
		catch(const std::exception& e) {
			logger << Logger::warn << e.what() << std::endl;
		}
	}
	this->load_page_content();
	this->define_status();
	this->_response_status = util::get_response_status(this->_status_code);

	if (this->_status_code == HTTP_MOVED_PERMANENTLY) {
		logger << Logger::info << "Redirecting\n";
		std::string new_location = this->request.location.get_redirect();
		this->_content_type = "text/html";
		this->_page_content = "<!DOCTYPE html>\r\n<html>\r\n<head>\r\n  <title>301 Moved Permanently</title>\r\n</head>\r\n<body>\r\n  <h1>301 Moved Permanently</h1>\r\n  <p>The page you requested has moved <a href=\"http://" + this->request.get_header("host") + new_location + "\">here</a>.</p>\r\n</body>\r\n</html>";
		std::string content_length = std::to_string(this->_page_content.length());
		this->_content_type = "text/html\r\nContent-Length: " + content_length + "\r\nLocation: http://" + this->request.get_header("host") + new_location;
		return ;
	}
	if (this->_status_code >= 400) {
		this->_filename = this->_file_dir + this->config.get_error_page(this->_status_code);
		this->load_page_content();
	}
	this->define_content_type();
}

std::string FileResponse::get_response(void) const {
	std::string response = this->request.http_version + " " + this->_response_status + CRLF + "Content-Type: " + this->_content_type + CRLF + CRLF + this->_page_content;
	// if (this->_status_code == HTTP_MOVED_PERMANENTLY) {
	// 	std::cout << "\nResponse: \n" << response << "\n\n";
	// }
	return response;
}
