#ifndef SERVER_HPP
 #define SERVER_HPP

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
	Server(int domain, int socketType, int protocol);

	void set_config(Config &config);

	void	set_address();
	void	bind(void);
	void	listen(int backlog);
	Client	&accept();
	void	close(void);
	Client	&get_client(int client_sockfd);
	int		get_sockfd(void) const;

private:
	int						_server_sockfd;
	int						_domain;
	int						_socket_type;
	int						_protocol;
	sockaddr_in				_address;
	int						_backlog;
	std::map<int, Client>	_clients;
	Config					*_config;
};

#endif
