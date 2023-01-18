#include "Client.hpp"
#include "EventQueue.hpp"
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

/* TODO: deze functie verbeteren, request doorsturen aan parser en evt aan CGI */
void Client::handle_event(struct kevent &ev_rec, EventQueue &event_queue) {
	if (ev_rec.filter == EVFILT_READ) {
		if (ev_rec.flags & EV_EOF) {
			std::cout << "Client disconnected: " << this->get_sockfd() << std::endl;
			this->close();
			event_queue.remove_event_listener(this->get_sockfd());
		}
		else {
			char buffer[1024];
			int bytes_read = read(this->get_sockfd(), buffer, sizeof(buffer));
			std::string request(buffer, bytes_read);

			std::cout << "Received request:" << std::endl;
			std::cout << request << std::endl;

			// Send the response
			std::string response = "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\nContent-Length: 13\r\n\r\nHello, world!";
			write(this->get_sockfd(), response.c_str(), response.size());
		}
	}
}
