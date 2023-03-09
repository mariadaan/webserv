#pragma once
#ifndef CLIENT_HPP
#define CLIENT_HPP

#include "NonBlockingRWStream.hpp"
#include "Response.hpp"

#include <netinet/in.h>
#include <sys/event.h>

class Server;
class Config;
class EventQueue;
class Client : private NonBlockingStreamReader, private NonBlockingStreamWriter {
public:
	Client(Config& config, Server& server, EventQueue& event_queue, int client_sockfd, sockaddr_in client_address);
	Config&		config;

	int			get_sockfd(void) const;
	void		close(void);
	std::string get_ip(void) const;
	void		handle_event(struct kevent& ev_rec);
	void		handle_cgi_event(struct kevent& ev_rec);
	void		send(std::string const& str);

private:
	Server&		_server;
	EventQueue&	_event_queue;
	int			_client_sockfd;
	sockaddr_in _client_address;

	Response _response;

	NonBlockingRWStream _stream_event_handler;

	virtual void _read_data(std::string str);
	virtual void _read_end();
	virtual void _write_end();
};

#endif
