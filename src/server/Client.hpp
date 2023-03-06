#pragma once
#ifndef CLIENT_HPP
#define CLIENT_HPP

#include "NonBlockingStream.hpp"
#include "Response.hpp"

#include <netinet/in.h>
#include <sys/event.h>

class Server;
class Config;
class EventQueue;
class Client : private NonBlockingStreamReader {
public:
	Client(Config& config, Server& server, int client_sockfd, sockaddr_in client_address);
	Config&		config;

	int			get_sockfd(void) const;
	void		close(void);
	std::string get_ip(void) const;
	void		handle_event(struct kevent& ev_rec, EventQueue& event_queue);
	void		handle_cgi_event(struct kevent& ev_rec);
	void		send(std::string const& str);

private:
	Server&		_server;
	int			_client_sockfd;
	sockaddr_in _client_address;

	Response _response;

	NonBlockingStream _stream_event_handler;

	virtual void nbsr_read_impl(std::string str, EventQueue& event_queue);
	virtual void nbsr_end();
};

#endif
