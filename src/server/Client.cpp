#include "Client.hpp"
#include "Logger.hpp"
#include "Server.hpp"
#include "EventQueue.hpp"
#include <fcntl.h>
#include <sstream>
#include <unistd.h>

Client::Client(Config& config, Server& server, EventQueue& event_queue, int client_sockfd, sockaddr_in client_address)
	: config(config)
	, _server(server)
	, _event_queue(event_queue)
	, _client_sockfd(client_sockfd)
	, _client_address(client_address)
	, _response(config, *this, event_queue)
	, _stream_event_handler(client_sockfd, *this, *this) {
	if (::fcntl(this->_client_sockfd, F_SETFL, O_NONBLOCK))
		throw(std::runtime_error("Error setting socket to non-blocking"));
}

Client::~Client() {
	this->_response.make_sure_cgi_done();
}

void Client::close(void) {
	logger << Logger::info << "Marking client for closing " << this->_client_sockfd << std::endl;
	this->_stream_event_handler.close();
}

void Client::send(std::string const& str) {
	this->_stream_event_handler.write(str);
}

int Client::get_sockfd(void) const {
	return (this->_client_sockfd);
}

std::string Client::get_ip(void) const {
	std::stringstream ss;
	uint32_t		  s_addr = ntohl(this->_client_address.sin_addr.s_addr);
	ss << ((s_addr >> 24) & 0xFF) << "." << ((s_addr >> 16) & 0xFF) << "."
	   << ((s_addr >> 8) & 0xFF) << "." << (s_addr & 0xFF);
	return (ss.str());
}

void Client::handle_event(struct kevent& ev_rec) {
	try {
		this->_stream_event_handler.handle_event(ev_rec);
	} catch (std::exception& e) {
		::close(this->_client_sockfd);
		this->_event_queue.remove_fd(this->_client_sockfd);
		this->_server.remove_client(this->_client_sockfd);
	}
}

void Client::handle_cgi_event(struct kevent& ev_rec) {
	try {
		this->_response.handle_cgi_event(ev_rec);
	} catch (std::exception& e) {
		::close(this->_client_sockfd);
		this->_event_queue.remove_fd(this->_client_sockfd);
		this->_server.remove_client(this->_client_sockfd);
	}
}

void Client::_read_data(std::string str) {
	this->_response.handle_part(str);
	if (this->_response.is_ready()) {
		if (this->_response.send()) {
			this->close();
		}
	}
}

void Client::_read_end() {
	logger << Logger::info << "Closing client " << this->_client_sockfd << std::endl;
	::close(this->_client_sockfd);
	this->_event_queue.remove_fd(this->_client_sockfd);
	this->_server.remove_client(this->_client_sockfd);
}

void Client::_write_end() {
	logger << Logger::info << "Closing client " << this->_client_sockfd << std::endl;
	::close(this->_client_sockfd);
	this->_event_queue.remove_fd(this->_client_sockfd);
	this->_server.remove_client(this->_client_sockfd);
}
