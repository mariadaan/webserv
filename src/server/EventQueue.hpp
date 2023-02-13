#pragma once
#ifndef EVENTQUEUE_HPP
# define EVENTQUEUE_HPP

#include <sys/types.h>
#include <sys/event.h>
#include <iostream>
#include <unistd.h>

#include <sys/socket.h>
#include <netinet/in.h>

#include <map>

#include "Server.hpp"

class EventQueue
{
public:
	EventQueue();

	void add_server(Server *server);
	void close_servers(void);

	void add_event_listener(int sockfd);
	void event_loop(void);

	void accept_client_on(Server &server);
	void remove_event_listener(int sockfd);

private:
	int _kq;
	std::map<int, Server *> _servers;
	std::map<int, int> _client_server;
};

#endif
