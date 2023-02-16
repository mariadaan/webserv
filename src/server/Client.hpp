#pragma once
#ifndef CLIENT_HPP
# define CLIENT_HPP

#include "Optional.hpp"
#include "ParsedRequest.hpp"
#include "CGI.hpp"
#include "Config.hpp"
#include "Response.hpp"

#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

class Chunk;
class EventQueue;
class Client
{
public:
	Client(Config &config, int client_sockfd, sockaddr_in client_address);

	Config		&config;
	int			get_sockfd(void) const;
	void		close(void);
	std::string	get_ip(void) const;
	void		handle_event(struct kevent &ev_rec);

private:
	int				_client_sockfd;
	sockaddr_in		_client_address;

	Response	_response;
};

#endif
