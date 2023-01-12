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
	if (::listen(_sockfd, _backlog) < 0)
		throw std::runtime_error("Error listening on socket");
}

void Socket::close(void)
{
	::close(this->_sockfd);
}

int Socket::get_sockfd(void) const
{
	return (this->_sockfd);
}