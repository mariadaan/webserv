#pragma once
#ifndef CLIENT_HPP
# define CLIENT_HPP

#include "Optional.hpp"
#include "ParsedRequest.hpp"
#include "CGI.hpp"
#include "Config.hpp"

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

	Optional<ParsedRequest>	_request;
	Optional<CGI>	_cgi;

	bool		_is_parsed(void) const;
	bool		_go_to_cgi(void) const;

	void		_handle_request(std::string const &received);
	void		_handle_chunks(std::string const &received);
	void		_handle_non_chunk(std::string const &received);
	void		_finish_request();
	void		_file_response();

	void		_wait_for_cgi();

	std::string &_get_body();
};

#endif
