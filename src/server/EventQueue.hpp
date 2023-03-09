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

class Server;
class EventQueue
{
public:
	EventQueue();

	void add_server(Server *server);
	void close_servers(void);

	void add_read_event_listener(int sockfd);
	void add_write_event_listener(int sockfd);
	void add_cgi_event_listener(int cgi_input_fd, int cgi_output_fd, int client_sockfd);
	void remove_fd(int fd);
	void event_loop(void);

	void accept_client_on(Server &server);

private:
	int _kq;
	std::map<int, Server *> _servers;
	std::map<int, int> _client_to_server;
	std::map<int, int> _cgi_to_client;
};

#endif
