#include "Client.hpp"
#include "Logger.hpp"
#include <fcntl.h>
#include <sstream>
#include <unistd.h>

Client::Client(Config& config, int client_sockfd, sockaddr_in client_address)
	: config(config)
	, _client_sockfd(client_sockfd)
	, _client_address(client_address)
	, _response(config, *this)
	, _can_write(false)
	, _want_to_write(false)
	, _close_state(OPEN) {
	if (::fcntl(this->_client_sockfd, F_SETFL, O_NONBLOCK))
		throw(std::runtime_error("Error setting socket to non-blocking"));
}

void Client::close(void) {
	this->_close_state = WANT_TO_CLOSE;
	this->_handle_state();
}

void Client::send(std::string const& str) {
	if (this->_close_state != OPEN) {
		throw std::runtime_error("Client: Trying to write to a closed client");
	}
	this->_want_to_write = true;
	this->_write_buffer += str;
	this->_handle_state();
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

void Client::_send_part() {
	size_t	size = this->_write_buffer.size();
	ssize_t size_sent = ::send(this->_client_sockfd, this->_write_buffer.c_str(), size, 0);
	if (size_sent == -1) {
		throw std::runtime_error("Error writing to socket");
	}
	this->_can_write = false;
	if ((size_t)size_sent == size) {
		this->_write_buffer = "";
		this->_want_to_write = false;
		return;
	}
	this->_write_buffer = this->_write_buffer.substr(size_sent);
	this->_want_to_write = true;
}

void Client::_handle_state() {
	if (!this->_can_write)
		return;

	if (this->_want_to_write) {
		if (this->_close_state == CLOSED) {
			throw std::runtime_error("Client: Trying to write to a closed client");
		}
		this->_send_part();
	}
	else {
		if (this->_close_state == WANT_TO_CLOSE) {
			::close(this->_client_sockfd);
			this->_close_state = CLOSED;
		}
	}
}

void Client::_handle_read_event(struct kevent& ev_rec) {
	if (ev_rec.flags & EV_EOF) {
		logger << Logger::info << "Client disconnected: " << this->get_sockfd()
			   << std::endl;
		this->close();
	}
	else {
		size_t		bytes_available = ev_rec.data;
		char*		buffer = new char[bytes_available];
		int			bytes_read = recv(this->get_sockfd(), buffer, bytes_available, 0);
		std::string received(buffer, bytes_read);
		delete[] buffer;

		this->_response.handle_part(received);
		if (this->_response.is_ready()) {
			this->_response.send();
			this->close();
		}
	}
}

void Client::_handle_write_event(struct kevent& ev_rec) {
	this->_can_write = true;
	this->_handle_state();
}

void Client::handle_event(struct kevent& ev_rec) {
	if (ev_rec.filter == EVFILT_READ) {
		this->_handle_read_event(ev_rec);
	}
	else if (ev_rec.filter == EVFILT_WRITE) {
		this->_handle_write_event(ev_rec);
	}
	else {
		// logger << Logger::error << "Unknown event filter: " << ev_rec.filter << std::endl;
		throw std::runtime_error("Client: Unknown event filter");
	}
}
