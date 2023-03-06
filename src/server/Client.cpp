#include "Client.hpp"
#include "Logger.hpp"
#include "Server.hpp"
#include <fcntl.h>
#include <sstream>
#include <unistd.h>

Client::Client(Config& config, Server& server, int client_sockfd, sockaddr_in client_address)
	: config(config)
	, _server(server)
	, _client_sockfd(client_sockfd)
	, _client_address(client_address)
	, _response(config, *this)
	, _stream_event_handler(client_sockfd, *this) {
	if (::fcntl(this->_client_sockfd, F_SETFL, O_NONBLOCK))
		throw(std::runtime_error("Error setting socket to non-blocking"));
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

void Client::handle_event(struct kevent& ev_rec, EventQueue& event_queue) {
	this->_stream_event_handler.handle_event(ev_rec, event_queue);
}

void Client::handle_cgi_event(struct kevent& ev_rec) {
	this->_response.handle_cgi_event(ev_rec);
}

void Client::nbsr_read_impl(std::string str, EventQueue& event_queue) {
	this->_response.handle_part(str);
	if (this->_response.should_add_cgi_to_event_queue()) {
		this->_response.add_cgi_to_event_queue(event_queue);
	}
	if (this->_response.is_ready()) {
		if (this->_response.send())
			this->close();
	}
}

void Client::nbsr_end() {
	logger << Logger::info << "Closing client " << this->_client_sockfd << std::endl;
	this->_server.remove_client(this->_client_sockfd);
}
