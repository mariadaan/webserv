#include "Client.hpp"
#include <arpa/inet.h>
#include <sstream>

Client::Client(int client_sockfd, sockaddr_in client_address, socklen_t client_address_len)
	: _client_sockfd(client_sockfd), _client_address(client_address), _client_address_len(client_address_len)
{
	(void)_client_address;
	(void)_client_address_len;
}

// TODO: should we also remove it from the map in the Socket class?
void Client::close(void) {
	::close(this->_client_sockfd);
}

int Client::get_sockfd(void) const {
	return (this->_client_sockfd);
}

std::string Client::get_ip(void) const {
	std::stringstream ss;
	uint32_t s_addr = ntohl(this->_client_address.sin_addr.s_addr);
	ss	<< ((s_addr & 0xFF000000) >> 24) << "."
		<< ((s_addr & 0xFF0000) >> 16) << "."
		<< ((s_addr & 0xFF00) >> 8) << "."
		<< (s_addr & 0xFF);
	return (ss.str());
}
