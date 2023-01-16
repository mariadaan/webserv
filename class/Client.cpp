#include "Client.hpp"
#include <sstream>

Client::Client(int client_sockfd, sockaddr_in client_address, socklen_t client_address_len)
	: _client_sockfd(client_sockfd), _client_address(client_address), _client_address_len(client_address_len)
{
}

// TODO: should we also remove it from the map in the Socket class?
void Client::close(void) {
	::close(this->_client_sockfd);
}

int Client::get_sockfd(void) const {
	return (this->_client_sockfd);
}