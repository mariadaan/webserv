#include "Response.hpp"
#include "Logger.hpp"
#include "FileResponse.hpp"
#include "Chunk.hpp"
#include "Client.hpp"
#include "EventQueue.hpp"
#include "defines.hpp"
#include "util.hpp"

#include <fcntl.h>
#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

Response::Response(Config &config, Client &client, EventQueue& event_queue)
	: _client(client)
	, _event_queue(event_queue)
	, _config(config)
	, _chunk_builder()
	, _request()
	, _cgi()
	, _ready(PARSING)
	, _status_code(HTTP_OK)
	, _body_size(0)
	, _cgi_buffer("") {
}

void Response::_mark_ready(void) {
	if (this->_ready == PARSING)
		this->_ready = READY;
}

bool Response::_headers_parsed(void) const {
	return (this->_request.is_set() && this->_request.headers_finished());
}

bool Response::_go_to_cgi(void) const {
	return (this->_cgi.is_set());
}

void Response::_send_file_response(void) {
	FileResponse file_response(this->_config, this->_request);
	std::string response = file_response.get_response();
	this->_client.send(response);
	logger << Logger::info << "Response sent" << std::endl;
}

void Response::_build_request(std::string const &received) {
	if (!this->_request.is_set()) {
		this->_request = Optional<ParsedRequest>(ParsedRequest(this->_config, received));
	}
	else {
		this->_request.parse_part(received);
	}
}

std::string Response::_get_status() {
	return util::get_response_status(this->_status_code);
}

bool Response::exceeds_max_body_size() {
	size_t max_body_size;

	if (this->_request.location.is_set() && this->_request.location.get_max_size() != 0) {
		max_body_size = this->_request.location.get_max_size();
	}
	else {
		max_body_size = this->_config.get_max_size();
	}
	if (max_body_size == 0) {
		return false;
	}
	if (this->_request.get_content_length() > max_body_size) {
		return true;
	}
	else {
		return false;
	}
}

void Response::_handle_request() {
	if (!this->_request.is_allowed_method) {
		if (this->_request.method != GET && this->_request.method != POST && this->_request.method != DELETE) {
			this->_status_code = HTTP_NOT_IMPLEMENTED;
		}
		else {
			this->_status_code = HTTP_METHOD_NOT_ALLOWED;
		}
	}

	logger << Logger::info << "Received request:\n" << this->_request << std::endl;
	if (this->_request.has_header("expect") != 0 && this->_request.headers["expect"] == "100-continue") {
		std::string response = "HTTP/1.1 " + util::get_response_status(HTTP_CONTINUE) + CRLF + CRLF;
		this->_client.send(response);
		logger << Logger::info << "response sent" << std::endl;
	}

	if (this->_request.is_chunked) {
		logger << Logger::info << "Chunked request" << std::endl;
	}
	else {
		if (!this->_request.has_header("content-length")) {
			if (this->_get_body().size() != 0) {
				this->_status_code = HTTP_BAD_REQUEST;
			}
			this->_get_body() = "";
			this->_mark_ready();
			if (this->_go_to_cgi()) {
				this->_cgi.end_of_input();
			}
		}
		else {
			if (this->_get_body().size() != 0) {
				std::string body = this->_get_body();
				this->_get_body() = "";
				this->_handle_body(body);
			}
		}
	}

	if (!this->has_error_status()) {
		if (!this->_request.get_script_name().empty()) {
			if (this->_request.has_header("content-length") && this->exceeds_max_body_size()) {
				logger << Logger::warn << "File too large! \n";
			}
			else {
				this->_cgi = Optional<CGI>(CGI(this->_request, *this, this->_client, this->_event_queue));
			}
		}
	} else {
		this->_mark_ready();
	}
}

std::string &Response::_get_body(void) {
	if (!this->_headers_parsed()) {
		throw std::logic_error("Response::get_body(): request not parsed");
	}
	return (this->_request.body);
}

void Response::_handle_body_chunks(std::string const &received) {
	logger << Logger::info << "Got chunk" << std::endl;
	std::vector<Chunk> chunks = this->_chunk_builder.parse(received);
	for (std::vector<Chunk>::const_iterator chunk_it = chunks.begin(); chunk_it < chunks.end(); chunk_it++) {
		if (chunk_it->get_size() == 0) {
			logger << Logger::info << "Got last chunk" << std::endl;
			this->_mark_ready();

			if (this->_go_to_cgi()) {
				this->_cgi.end_of_input();
			}
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
		this->_mark_ready();
	}

	if (this->_go_to_cgi()) {
		this->_cgi.write(received.c_str(), receive_size);
		if (this->_body_size >= this->_request.get_content_length()) {
			this->_cgi.end_of_input();
		}
	}
	else {
		if (receive_size != received.size()) {
			this->_get_body() += received.substr(0, receive_size);
		}
		else {
			this->_get_body() += received;
		}
	}
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
	this->_client.send(content_type_header);
	std::string header_end = CRLF;
	this->_client.send(header_end);

	this->_client.send(page_content);
}

void Response::_send_status(HTTP_STATUS_CODES status_code) {
	std::string status = util::get_response_status(status_code);
	std::string response = "HTTP/1.1 " + status + CRLF;
	this->_client.send(response);
}

void Response::_send_status() {
	this->_send_status(this->_status_code);
}

bool Response::send() {
	if (this->_ready == SENT) {
		return (false);
	}
	this->_ready = SENT;
	if (this->has_error_status()) {
		logger << Logger::info << "Sending error response "<< this->_status_code << std::endl;
		this->_send_error_response();
		return (true);
	}

	if (this->_go_to_cgi()) {
		logger << Logger::debug << "Going to CGI" << std::endl;
		return (false);
	}
	else {
		this->_send_file_response();
	}
	return (true);
}

bool Response::is_ready() const {
	return (this->_ready == READY);
}

bool Response::has_error_status() const {
	return (this->_status_code >= 400);
}

void Response::handle_cgi_output(std::string const& str) {
	this->_cgi_buffer += str;
}

void Response::handle_cgi_end() {
	this->_status_code = this->_cgi.wait();
	if (this->has_error_status()) {
		this->_send_error_response();
	}
	else {
		this->_send_status();
		this->_client.send(this->_cgi_buffer);
	}
	this->_client.close();
}

void Response::handle_cgi_event(struct kevent& ev_rec) {
	this->_cgi.handle_event(ev_rec);
}
