#include "FileResponse.hpp"
#include "Logger.hpp"
#include "util.hpp"
#include "defines.hpp"

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
		throw std::runtime_error("Error opening directory");
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
	if (this->request.location.get_index().empty() && this->request.location.get_auto_index()) {
		logger << Logger::debug << "autoindexing" << std::endl;
		this->_response_status = HTTP_OK;
		this->_content_type = "text/html";
		this->directory_listing();
		return ;
	}
	this->load_page_content();
	this->define_status();
	this->_response_status = util::get_response_status(this->_status_code);

	if (this->_status_code >= 400) {
		this->_filename = this->_file_dir + this->config.get_error_page(this->_status_code);
		this->load_page_content();
	}
	this->define_content_type();
}

std::string FileResponse::get_response(void) const {
	return this->request.http_version + " " + this->_response_status + CRLF + "Content-Type: " + this->_content_type + CRLF + CRLF + this->_page_content;
}
