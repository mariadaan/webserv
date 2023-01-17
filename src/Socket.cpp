#include "Socket.hpp"

Socket::Socket(int domain, int socketType, int protocol)
	: _domain(domain), _socket_type(socketType), _protocol(protocol)
{
	this->_sockfd = socket(_domain, _socket_type, _protocol);
	if (this->_sockfd < 0)
		throw std::runtime_error("Error creating socket");
}

void Socket::set_address(int portnum)
{
	this->_portnum = portnum;
	std::memset(&_address, 0, sizeof(_address)); // pad the structure to the length of a struct sockaddr. set to zero
	_address.sin_family = AF_INET; // IPv4
	_address.sin_addr.s_addr = INADDR_ANY; // local address
	_address.sin_port = htons(_portnum); // convert to network byte order
}

void Socket::bind(void)
{
	if (::bind(_sockfd, (sockaddr*) &_address, sizeof(_address)) < 0)
		throw std::runtime_error("Error binding socket");
}

void Socket::listen(int backlog)
{
	this->_backlog = backlog;
	if (::listen(this->_sockfd, this->_backlog) < 0)
		throw std::runtime_error("Error listening on socket");
}

Client &Socket::accept()
{
	sockaddr_in client_address;
	socklen_t client_len = sizeof(client_address);
	int client_sockfd = ::accept(this->_sockfd, (sockaddr*) &client_address, &client_len);
	if (client_sockfd < 0)
		throw std::runtime_error("Error accepting connection");
	Client client(client_sockfd, client_address, client_len);
	this->_clients[client_sockfd] = client;
	return this->_clients.at(client_sockfd);
}

void Socket::close(void)
{
	::close(this->_sockfd);
}

Client &Socket::get_client(int client_sockfd)
{
	return this->_clients.at(client_sockfd);
}

int Socket::get_sockfd(void) const
{
	return (this->_sockfd);
}
