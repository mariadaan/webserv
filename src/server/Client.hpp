#pragma once
#ifndef CLIENT_HPP
#define CLIENT_HPP

#include "Config.hpp"
#include "Response.hpp"

#include <netinet/in.h>
#include <sys/event.h>

class Client {
public:
	Client(Config& config, int client_sockfd, sockaddr_in client_address);

	Config&		config;
	int			get_sockfd(void) const;
	void		close(void);
	std::string get_ip(void) const;
	void		handle_event(struct kevent& ev_rec);

	void		send(std::string const& str);

private:
	int			_client_sockfd;
	sockaddr_in _client_address;

	Response _response;

	bool _can_write;
	bool _want_to_write;

	// enum {
	// 	READY_TO_WRITE,
	// 	WANT_TO_WRITE
	// } 			_write_state;
	enum {
		OPEN,
		WANT_TO_CLOSE,
		CLOSED
	} 			_close_state;
	std::string	_write_buffer;

	void _send_part();
	void _handle_state();
	void _handle_read_event(struct kevent& ev_rec);
	void _handle_write_event(struct kevent& ev_rec);
};

#endif
