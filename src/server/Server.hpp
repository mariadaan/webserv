#pragma once
#ifndef SERVER_HPP
# define SERVER_HPP

#include "Client.hpp"
#include "Config.hpp"

#include <map>
#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

class Server
{
public:
	Server(Config& config, EventQueue& event_queue, int domain, int socketType, int protocol);
	~Server();

	Config	&config;

	void	set_address();
	void	bind(void);
	void	listen(int backlog);
	Client	&accept();
	void	close(void);
	Client	&get_client(int client_sockfd);
	std::map<int, Client *>	&get_clients();
	int		get_sockfd(void) const;
	void	remove_client(int client_sockfd);

private:
	EventQueue&	_event_queue;

	int			_server_sockfd;
	int			_domain;
	int			_socket_type;
	int			_protocol;
	sockaddr_in	_address;
	int			_backlog;

	std::map<int, Client*>	_clients;
};

#endif
