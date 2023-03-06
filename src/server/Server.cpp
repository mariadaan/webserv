#include "Server.hpp"
#include "Client.hpp"
#include "Logger.hpp"
#include "Config.hpp"
#include <fcntl.h>

Server::Server(Config &config, int domain, int socketType, int protocol)
	: config(config)
	, _domain(domain)
	, _socket_type(socketType)
	, _protocol(protocol)
{
	this->_server_sockfd = socket(_domain, _socket_type, _protocol);
	if (this->_server_sockfd < 0)
		throw std::runtime_error("Error creating socket");
	if (::fcntl(this->_server_sockfd, F_SETFL, O_NONBLOCK))
		throw (std::runtime_error("Error setting socket to non-blocking"));
}

Server::~Server() {
	for (std::map<int, Client *>::iterator it = this->_clients.begin(); it != this->_clients.end(); it++) {
		delete it->second;
	}
}

void Server::set_address(void) {
	std::memset(&_address, 0, sizeof(_address));		// pad the structure to the length of a struct sockaddr. set to zero
	_address.sin_family = AF_INET;						// IPv4
	_address.sin_addr.s_addr = INADDR_ANY;				// local address
	_address.sin_port = htons(this->config.get_port()); // convert to network byte order
}

void Server::bind(void) {
	if (::bind(this->_server_sockfd, (sockaddr *)&_address, sizeof(_address)) < 0)
		throw std::runtime_error("Error binding socket");
}

void Server::listen(int backlog) {
	this->_backlog = backlog;
	if (::listen(this->_server_sockfd, this->_backlog) < 0)
		throw std::runtime_error("Error listening on socket");
}

Client &Server::accept() {
	sockaddr_in client_address;
	socklen_t client_len = sizeof(client_address);
	int client_sockfd = ::accept(this->_server_sockfd, (sockaddr *)&client_address, &client_len);
	if (client_sockfd < 0)
		throw std::runtime_error("Error accepting connection");
	Client *client = new Client(this->config, *this, client_sockfd, client_address);
	this->_clients[client_sockfd] = client;
	return *(this->_clients.at(client_sockfd));
}

void Server::close(void) {
	::close(this->_server_sockfd);
	logger << Logger::info << "Closed server sockfd " << this->get_sockfd() << std::endl;
}

Client &Server::get_client(int client_sockfd) {
	return *(this->_clients.at(client_sockfd));
}

int Server::get_sockfd(void) const {
	return (this->_server_sockfd);
}

std::map<int, Client *> &Server::get_clients() {
	return this->_clients;
}

void Server::remove_client(int client_sockfd) {
	delete this->_clients.at(client_sockfd);
	this->_clients.erase(client_sockfd);
}
