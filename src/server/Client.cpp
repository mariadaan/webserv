#include "Client.hpp"
#include "EventQueue.hpp"
#include "ParsedRequest.hpp"
#include "FileResponse.hpp"
#include "Chunk.hpp"
#include "util.hpp"
#include "Logger.hpp"
#include <sstream>

Client::Client(int client_sockfd, sockaddr_in client_address)
	: _client_sockfd(client_sockfd), _client_address(client_address)
{}

// TODO: should we also remove it from the map in the Server class?
void Client::close(void) {
	::close(this->_client_sockfd);
}

int Client::get_sockfd(void) const {
	return (this->_client_sockfd);
}

std::string Client::get_ip(void) const {
	std::stringstream ss;
	uint32_t s_addr = ntohl(this->_client_address.sin_addr.s_addr);
	ss	<< ((s_addr >> 24) & 0xFF) << "."
		<< ((s_addr >> 16) & 0xFF) << "."
		<< ((s_addr >>  8) & 0xFF) << "."
		<< ( s_addr        & 0xFF);
	return (ss.str());
}

bool Client::_is_parsed(void) const {
	return (this->_request.is_set() && this->_request.headers_finished());
}

bool Client::_go_to_cgi(void) const {
	return (this->_cgi.is_set());
}

// when the request does not include a cgi
void Client::_file_response(void) {
	FileResponse file_response(this->_request.path);
	file_response.generate_response();
	::send(this->get_sockfd(), file_response.get_response().c_str(), file_response.get_response().size(), 0);
	logger << Logger::info << "Response sent" << std::endl;
}

void Client::_handle_request(std::string const &received) {
	if (!this->_request.is_set()) {
		this->_request = Optional<ParsedRequest>(ParsedRequest(received));
	}
	else {
		this->_request.parse_part(received);
	}

	if (!this->_is_parsed()) {
		return;
	}

	logger << Logger::info << "Received request:\n" << this->_request << std::endl;

	if (!this->_request.get_script_name().empty()) {
		this->_cgi = Optional<CGI>(CGI(this->_request, *this));
	}
	else {
		// TODO: maybe do something for non-cgi requests
	}

	if (this->_request.has_header("expect") != 0 && this->_request.headers["expect"] == "100-continue") {
		std::string response = "HTTP/1.1 100 Continue\r\n\r\n";
		::send(this->get_sockfd(), response.c_str(), response.size(), 0);
		logger << Logger::info << "response sent" << std::endl;
	}

	if (this->_request.is_chunked) {
		logger << Logger::info << "Chunked request" << std::endl;
		logger << Logger::info << "done" << std::endl;
	}
	else {
		if (!this->_request.has_header("content-length")) {
			if (this->_get_body().size() != 0) {
				// TODO: send 400: Bad Request
			}
			this->_get_body() = "";
			this->_finish_request();
		}
		else {
			if (this->_get_body().size() == this->_request.get_content_length()) {
				this->_finish_request();
			}
		}
	}
}

void Client::_wait_for_cgi() {
	std::string response = "HTTP/1.1 200 OK\r\n"; // https://www.rfc-editor.org/rfc/rfc3875#section-6.2.1
	::send(this->get_sockfd(), response.c_str(), response.size(), 0);
	this->_cgi.wait(); // currently also closes the write end of the pipe
}

std::string &Client::_get_body(void) {
	if (!this->_is_parsed()) {
		throw std::logic_error("Client::get_body(): request not parsed");
	}
	return (this->_request.body);
}

void Client::_finish_request() {
	if (this->_go_to_cgi()) {
		logger << Logger::warn << "Going to CGI" << std::endl;
		if (!this->_request.is_chunked) {
			this->_cgi.write(this->_request.body.c_str(), this->_request.body.size());
		}
		this->_wait_for_cgi();
	}
	else {
		this->_file_response();
	}
	this->close();
}

void Client::_handle_chunks(std::string const &received) {
	logger << Logger::info << "Got chunk" << std::endl;
	std::vector<Chunk> chunks = get_chunks(received);
	for (std::vector<Chunk>::const_iterator chunk_it = chunks.begin(); chunk_it < chunks.end(); chunk_it++) {
		if (chunk_it->get_size() == 0) {
			logger << Logger::info << "Got last chunk" << std::endl;
			this->_finish_request();
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

void Client::_handle_non_chunk(std::string const &received) {
	this->_get_body() += received;
	logger << Logger::info;
	logger << "Got part:" << std::endl;
	logger << "---------------" << std::endl;
	logger << received << std::endl;
	logger << "---------------" << std::endl;
	if (this->_get_body().size() >= this->_request.get_content_length()) {
		this->_get_body().resize(this->_request.get_content_length()); // TODO: check if ok
		this->_finish_request();
	}
}

void Client::handle_event(struct kevent &ev_rec) {
	if (ev_rec.filter == EVFILT_READ) {
		if (ev_rec.flags & EV_EOF) {
			logger << Logger::info << "Client disconnected: " << this->get_sockfd() << std::endl;
			this->close();
		}
		else {
			size_t bytes_available = ev_rec.data;
			char *buffer = new char[bytes_available];
			int bytes_read = recv(this->get_sockfd(), buffer, bytes_available, 0);
			std::string received(buffer, bytes_read);
			delete[] buffer;

			if (!this->_is_parsed()) {
				this->_handle_request(received);
			}
			else {
				if (this->_request.is_chunked)
					this->_handle_chunks(received);
				else
					this->_handle_non_chunk(received);
			}
		}
	}
}