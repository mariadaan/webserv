#pragma once
#ifndef CLIENT_HPP
#define CLIENT_HPP

#include "Response.hpp"

#include <netinet/in.h>
#include <sys/event.h>

class Server;
class Config;
class Client {
public:
	Client(Config& config, Server& server, int client_sockfd, sockaddr_in client_address);

	int			get_sockfd(void) const;
	void		close(void);
	std::string get_ip(void) const;
	void		handle_event(struct kevent& ev_rec);

	void		send(std::string const& str);

private:
	Config&		_config;
	Server&		_server;
	int			_client_sockfd;
	sockaddr_in _client_address;

	Response _response;

	bool _want_to_write;
	enum {
		CANNOT_WRITE,
		CAN_WRITE,
		WRITE_EOF
	}	_write_state;
	enum {
		OPEN,
		WANT_TO_CLOSE,
		CLOSED
	} 			_close_state;
	std::string	_write_buffer;

	void _end();
	void _send_part();
	void _handle_state();
	void _handle_read_event(struct kevent& ev_rec);
	void _handle_write_event(struct kevent& ev_rec);
};

#endif
