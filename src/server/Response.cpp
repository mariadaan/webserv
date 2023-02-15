#include "Response.hpp"
#include "Logger.hpp"
#include "FileResponse.hpp"
#include "Chunk.hpp"
#include "Client.hpp"
#include "defines.hpp"
#include "util.hpp"

#include <fcntl.h>
#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

Response::Response(Config &config, Client &client) : _client(client), _config(config), _ready(false), _status_code(HTTP_OK) {
}

bool Response::_headers_parsed(void) const {
	return (this->_request.is_set() && this->_request.headers_finished());
}

bool Response::_go_to_cgi(void) const {
	return (this->_cgi.is_set());
}

// when the request does not include a cgi
void Response::_send_file_response(void) {
	FileResponse file_response(this->_config, this->_request);
	std::string response = file_response.get_response();
	::send(this->_client.get_sockfd(), response.c_str(), response.size(), 0);
	logger << Logger::info << "Response sent" << std::endl;
	// logger << Logger::debug << response << std::endl;;
}

void Response::_build_request(std::string const &received) {
	if (!this->_request.is_set()) {
		this->_request = Optional<ParsedRequest>(ParsedRequest(received));
	}
	else {
		this->_request.parse_part(received);
	}
}

std::string Response::_get_status() {
	return util::get_response_status(this->_status_code);
}

void Response::_handle_request() {
	this->_request.set_location(this->_config.get_locations());

	if (!this->_request.is_allowed_method) {
		logger << Logger::info << "Method not allowed" << std::endl;

		this->_status_code = HTTP_METHOD_NOT_ALLOWED;
		this->_ready = true;
		return;
	}

	logger << Logger::info << "Received request:\n" << this->_request << std::endl;

	if (!this->_request.get_script_name().empty()) {
		this->_cgi = Optional<CGI>(CGI(this->_request, this->_client));
	}
	else {
		// TODO: maybe do something for non-cgi requests
	}

	if (this->_request.has_header("expect") != 0 && this->_request.headers["expect"] == "100-continue") {
		std::string response = "HTTP/1.1 " + util::get_response_status(HTTP_CONTINUE) + CRLF + CRLF;
		::send(this->_client.get_sockfd(), response.c_str(), response.size(), 0);
		logger << Logger::info << "response sent" << std::endl;
	}

	if (this->_request.is_chunked) {
		logger << Logger::info << "Chunked request" << std::endl;
	}
	else {
		if (!this->_request.has_header("content-length")) {
			if (this->_get_body().size() != 0) {
				// TODO: send 400: Bad Request
				// this->_status_code = HTTP_BAD_REQUEST;
			}
			this->_get_body() = "";
			// this->_finish_request();
			this->_ready = true;
		}
		else {
			if (this->_get_body().size() == this->_request.get_content_length()) {
				// this->_finish_request();
				this->_ready = true;
			}
		}
	}
}

void Response::_wait_for_cgi() {
	this->_send_status(); // https://www.rfc-editor.org/rfc/rfc3875#section-6.2.1
	// std::string response = "HTTP/1.1 200 OK" CRLF; // https://www.rfc-editor.org/rfc/rfc3875#section-6.2.1
	// ::send(this->_client.get_sockfd(), response.c_str(), response.size(), 0);
	this->_cgi.wait(); // currently also closes the write end of the pipe
}

std::string &Response::_get_body(void) {
	if (!this->_headers_parsed()) {
		throw std::logic_error("Response::get_body(): request not parsed");
	}
	return (this->_request.body);
}

void Response::_handle_body_chunks(std::string const &received) {
	logger << Logger::info << "Got chunk" << std::endl;
	std::vector<Chunk> chunks = get_chunks(received);
	for (std::vector<Chunk>::const_iterator chunk_it = chunks.begin(); chunk_it < chunks.end(); chunk_it++) {
		if (chunk_it->get_size() == 0) {
			logger << Logger::info << "Got last chunk" << std::endl;
			// this->_finish_request();
			this->_ready = true;
			return ;
		}

		if (this->_go_to_cgi()) {
			this->_cgi.write(chunk_it->get_content().c_str(), chunk_it->get_size());
		}
		else {
			this->_get_body() += chunk_it->get_content();
		}
	}
}

void Response::_handle_body(std::string const &received) {
	this->_body_size += received.size();

	size_t receive_size = received.size();

	if (this->_body_size >= this->_request.get_content_length()) {
		receive_size = received.size() - (this->_body_size - this->_request.get_content_length());
		this->_ready = true;
	}

	if (this->_go_to_cgi()) {
		this->_cgi.write(received.c_str(), receive_size);
	}
	else {
		if (receive_size != received.size()) {
			this->_get_body() += received.substr(0, receive_size);
		}
		else {
			this->_get_body() += received;
		}
	}

	// if (this->_body_size >= this->_request.get_content_length()) {
	// 	received.resize(this->_request.get_content_length() - this->_body_size);
	// }


	// this->_get_body() += received;
	// logger << Logger::info;
	// logger << "Got part:" << std::endl;
	// logger << "---------------" << std::endl;
	// logger << received << std::endl;
	// logger << "---------------" << std::endl;
	// if (this->_get_body().size() >= this->_request.get_content_length()) {
	// 	this->_get_body().resize(this->_request.get_content_length()); // TODO: check if ok
	// 	// this->_finish_request();
	// 	this->_ready = true;
	// }
}

void Response::handle_part(std::string const &received) {
	if (!this->_headers_parsed()) {
		this->_build_request(received);
		if (this->_headers_parsed()) {
			this->_handle_request();
		}
	}
	else {
		if (this->_request.is_chunked)
			this->_handle_body_chunks(received);
		else
			this->_handle_body(received);
	}
}

void Response::_send_error_response() {
	this->_send_status();

	std::string error_page = this->_config.get_root() + "/" + this->_config.get_error_page(this->_status_code);
	std::string content_type = util::get_content_type(util::get_file_extension(error_page));
	std::string page_content;
	if (util::can_open_file(error_page)) {
		page_content = util::file_to_str(error_page);
	}
	else {
		logger << Logger::warn << "Error page not found" << std::endl;
	}

	std::string content_type_header = "Content-Type: " + content_type + CRLF;
	::send(this->_client.get_sockfd(), content_type_header.c_str(), content_type_header.size(), 0);
	std::string header_end = CRLF;
	::send(this->_client.get_sockfd(), header_end.c_str(), header_end.size(), 0);

	::send(this->_client.get_sockfd(), page_content.c_str(), page_content.size(), 0);

	logger << Logger::debug << "Sent error response" << std::endl;
}

void Response::_send_status(HTTP_STATUS_CODES status_code) {
	std::string status = util::get_response_status(status_code);
	std::string response = "HTTP/1.1 " + status + CRLF;
	::send(this->_client.get_sockfd(), response.c_str(), response.size(), 0);
}

void Response::_send_status() {
	this->_send_status(this->_status_code);
}

void Response::send() {
	if (this->has_error_status()) {
		logger << Logger::info << "Sending error response" << std::endl;
		this->_send_error_response();
		return ;
	}

	if (this->_go_to_cgi()) {
		logger << Logger::debug << "Going to CGI" << std::endl;
		// if (!this->_request.is_chunked) {
		// 	this->_cgi.write(this->_request.body.c_str(), this->_request.body.size());
		// }
		this->_wait_for_cgi();
	}
	else {
		this->_send_file_response();
	}
}

bool Response::is_ready() const {
	return this->_ready;
}

bool Response::has_error_status() const {
	return (this->_status_code >= 400);
}
