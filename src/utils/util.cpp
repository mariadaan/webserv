#include "util.hpp"

namespace util {
	std::vector<std::string> split_string(std::string const &str, std::string const &delim) {
		std::vector<std::string> result;

		size_t pos = 0;
		while (true) {
			size_t found = str.find(delim, pos);

			if (found == std::string::npos) {
				result.push_back(str.substr(pos));
				return result;
			}
			result.push_back(str.substr(pos, found - pos));
			pos = found + delim.length();
		}
	}

	unsigned char char_to_lower(char c) {
		return std::tolower(c);
	}

	void str_to_lower(std::string &str) {
		std::transform(str.begin(), str.end(), str.begin(), &char_to_lower);
	}

	bool can_open_file(const std::string& filename) {
		std::ifstream file(filename);
		if (file.is_open()) {
			file.close();
			return true;
		}
		else {
			return false;
		}
	}

	std::string file_to_str(std::string const &filename) {
		std::ifstream file(filename);
		std::string str;
		if(file) {
			std::ostringstream ss;
			ss << file.rdbuf();
			str = ss.str();
		}
		return str;
	}

	std::string get_file_extension(std::string filename) {
		if (filename.find_last_of(".") == std::string::npos || filename.find_last_of(".") == 0) {
			return "txt";
		}
		else {
			return filename.substr(filename.find_last_of(".") + 1);
		}
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

	std::string get_response_status(HTTP_STATUS_CODES status_code) {
		std::map<HTTP_STATUS_CODES, std::string> status_codes;

		status_codes[HTTP_CONTINUE] = "100 Continue";
		status_codes[HTTP_OK] = "200 OK";
		status_codes[HTTP_CREATED] = "201 Created";
		status_codes[HTTP_NO_CONTENT] = "204 No Content";
		// status_codes[HTTP_BAD_REQUEST] = "400 Bad Request";
		// status_codes[HTTP_UNAUTHORIZED] = "401 Unauthorized";
		// status_codes[HTTP_FORBIDDEN] = "403 Forbidden";
		status_codes[HTTP_NOT_FOUND] = "404 Not Found";
		status_codes[HTTP_METHOD_NOT_ALLOWED] = "405 Method Not Allowed";
		// status_codes[HTTP_INTERNAL_SERVER_ERROR] = "500 Internal Server Error";
		// status_codes[HTTP_SERVICE_UNAVAILABLE] = "503 Service Unavailable";

		return status_codes.at(status_code);
	}
}
