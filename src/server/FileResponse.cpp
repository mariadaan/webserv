#include "FileResponse.hpp"
#include "Logger.hpp"
#include "util.hpp"
#include "defines.hpp"
#include <cstdio>

FileResponse::FileResponse(Config &config, ParsedRequest &request) : config(config), request(request) {
	this->_file_dir = config.get_root() + "/";
	if (this->request.location.is_set() && !this->request.location.get_root().empty()) {
		logger << Logger::info << "Different root found: " << this->request.location.get_root() << std::endl;
		this->_file_dir = this->request.location.get_root() + "/";
	}
	this->_filename = this->_file_dir + this->request.path;
	std::string index_file = this->_filename + "/" + this->request.location.get_index();
	if (this->request.method != POST && this->request.location.is_set() && !this->request.location.get_index().empty() && util::can_open_file(index_file)) {
		this->_filename = index_file;
	}
	else {
		this->_filename = this->_file_dir + this->request.path;
	}
	util::replace_all(this->_filename, "%20", " ");
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
	bool can_open = util::is_file(this->_filename.c_str());
	// if (!can_open) {
	// 	logger << Logger::warn << "Could not open " << this->_filename << std::endl;
	// }
	return can_open;
}

void FileResponse::load_page_content(void) {
	this->_file_accessible = this->can_open_file();
	if (this->_file_accessible) {
		this->_page_content = util::file_to_str(this->_filename);
	}
}

void FileResponse::define_status(void) {
	if (this->request.location.is_set() && !this->request.location.get_redirect().empty()) {
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

	try {
		this->_content_type = util::get_content_type(file_extension);
	}
	catch (const std::exception& e) {
		logger << Logger::warn << "Requested file type with extension \"" << file_extension << "\" invalid." << std::endl;
	}
}

void FileResponse::delete_files(std::string path) {
	DIR* dir = opendir(this->_filename.c_str());
	if (dir == nullptr) {
		throw std::runtime_error("Error opening directory: " + this->_filename);
	}
	std::string deleted_files;
	struct dirent* entry;
	while ((entry = readdir(dir)) != nullptr) {
		if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
			continue;
		}
		std::string filename(entry->d_name);
		std::string full_filename = path + "/" + filename;
		std::remove(full_filename.c_str());
		deleted_files += "<p>" + filename + "</p>\r\n";
	}
	if (deleted_files.empty()) {
		this->_page_content = "<h3>No files to delete!</h3>\r\n";
	}
	else {
		this->_page_content += "<h3>Deleted the following files: </h3>\r\n" + deleted_files;
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

bool FileResponse::exceeds_max_body_size() {
	size_t max_body_size;

	if (this->request.location.is_set() && this->request.location.get_max_size() != 0) {
		max_body_size = this->request.location.get_max_size();
	}
	else {
		max_body_size = this->config.get_max_size();
	}
	if (max_body_size == 0) {
		return false;
	}
	if (this->request.get_content_length() > max_body_size) {
		return true;
	}
	else {
		return false;
	}
}

void FileResponse::generate_response(void) {
	if (this->_auto_indexing) {
		this->_status_code = HTTP_OK;
		this->_response_status = util::get_response_status(HTTP_OK);
		this->_content_type = "text/html";
		try {
			this->directory_listing();
			return ;
		}
		catch(const std::exception& e) {
			logger << Logger::warn << e.what() << std::endl;
		}
	}
	if (this->request.method == DELETE && this->request.is_allowed_method == true) {
		this->_status_code = HTTP_OK;
		this->_response_status = util::get_response_status(HTTP_OK);
		this->_content_type = "text/html";
		try {
			logger << Logger::debug << "Deleting files now\n";
			this->delete_files(this->_filename);
			return;
		}
		catch(const std::exception& e) {
			logger << Logger::warn << "Folder " << this->_filename << " does not exist. No files to be deleted." << '\n';
		}
	}
	if (this->request.method == POST && this->request.is_allowed_method == true) {
		this->_content_type = "text/plain";
		if (this->exceeds_max_body_size()) {
			this->_status_code = HTTP_ENTITY_TOO_LARGE;
			this->_page_content = "Request entity too large\r\n";
		}
		else {
			this->_status_code = HTTP_OK;
			this->_page_content = "Received message: " + this->request.body + CRLF;
		}
		this->_response_status = util::get_response_status(this->_status_code);
		return ;
	}
	this->load_page_content();
	this->define_status();
	this->_response_status = util::get_response_status(this->_status_code);

	if (this->_status_code == HTTP_MOVED_PERMANENTLY) {
		logger << Logger::info << "Redirecting\n";
		std::string new_location = this->request.location.get_redirect();
		this->_content_type = "text/html";
		this->_page_content = "<head>\r\n  <title>301 Moved Permanently</title>\r\n</head>\r\n<body>\r\n  <h1>301 Moved Permanently</h1>\r\n  <p>The page you requested has moved <a href=\"http://" + this->request.get_header("host") + new_location + "\">here</a>.</p>\r\n</body>\r\n</html>";
		std::string content_length = std::to_string(this->_page_content.length());
		this->_content_type = "text/html\r\nContent-Length: " + content_length + "\r\nLocation: http://" + this->request.get_header("host") + new_location;
		return ;
	}
	if (this->_status_code >= 400) {
		this->_file_dir = config.get_root() + "/";
		try {
			this->_filename = this->_file_dir + this->config.get_error_page(this->_status_code);
			this->load_page_content();
		}
		catch(const std::exception& e) {
			this->_filename = "";
			this->_page_content = "Error " + std::to_string(this->_status_code);
		}
	}
	this->define_content_type();
}

HTTP_STATUS_CODES FileResponse::get_status() const {
	return (this->_status_code);
}

std::string FileResponse::get_response(void) const {
	std::string response = this->request.http_version + " " + this->_response_status + CRLF + "Content-Type: " + this->_content_type + CRLF + CRLF + this->_page_content;
	logger << Logger::info << "Response sent with status " << this->_response_status << std::endl;
	return response;
}
