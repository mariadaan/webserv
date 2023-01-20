#include "Client.hpp"
#include "EventQueue.hpp"
#include "ParsedRequest.hpp"
#include <sstream>

Client::Client(int client_sockfd, sockaddr_in client_address, socklen_t client_address_len)
	: _client_sockfd(client_sockfd), _client_address(client_address), _client_address_len(client_address_len)
{}

// TODO: should we also remove it from the map in the Server class?
void Client::close(void) {
	::close(this->_client_sockfd);
}

int Client::get_sockfd(void) const {
	return (this->_client_sockfd);
}

bool Client::is_parsed(void) const {
	return (this->_request.is_set());
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

/* TODO: deze functie verbeteren, request doorsturen aan parser en evt aan CGI */
void Client::handle_event(struct kevent &ev_rec, EventQueue &event_queue) {
	if (ev_rec.filter == EVFILT_READ) {
		if (ev_rec.flags & EV_EOF) {
			std::cout << "Client disconnected: " << this->get_sockfd() << std::endl;
			this->close();
		}
		else {
			size_t bytes_available = ev_rec.data + 1;
			char buffer[bytes_available];
			int bytes_read = recv(this->get_sockfd(), buffer, bytes_available, 0);
			std::string received(buffer, bytes_read);

			if (this->is_parsed()) {
				if (!this->_request.is_chunked)
					throw std::logic_error("Received data while (non-chunked) request was already parsed");
				std::cout << "Received chunked data:" << std::endl;
				std::cout << received << std::endl;
				// TODO: handle chunked data
			}
			else {
				this->_request = Optional<ParsedRequest>(ParsedRequest(received));

				std::cout << "Received request:" << std::endl;
				std::cout << this->_request << std::endl;

				if (this->_request.is_chunked) {
					std::cout << "Chunked request" << std::endl;
					if (this->_request.headers.count("expect") != 0 && this->_request.headers["expect"] == "100-continue") {
						std::string response = "HTTP/1.1 100 Continue\r\n\r\n";
						send(this->get_sockfd(), response.c_str(), response.size(), 0);
					}
				}
				else {
					std::cout << "Not chunked request" << std::endl;
					// Send the response
					std::string response = "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\nContent-Length: 13\r\n\r\nHello, world!";
					send(this->get_sockfd(), response.c_str(), response.size(), 0);
				}
			}
		}
	}
}
