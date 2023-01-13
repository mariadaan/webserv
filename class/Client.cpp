#include "Client.hpp"

Client::Client(void) {
	this->_client_address_len = sizeof(this->_client_address);
}

void Client::accept(int server_sockfd) {
	this->_client_sockfd = ::accept(server_sockfd, (sockaddr*) &_client_address, &_client_address_len);
	if (this->_client_sockfd <= 0)
		throw std::runtime_error("Error accepting incoming connections on listening socket");
}

void Client::close(void) {
	::close(this->_client_sockfd);
}

int Client::get_sockfd(void) const {
	return (this->_client_sockfd);
}