#ifndef SOCKET_HPP
 #define SOCKET_HPP

#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

class Socket
{
public:
	Socket(int domain, int socketType, int protocol);

	void	set_address(int portnum);
	void	bind(void);
	void	listen(int backlog);
	void	close(void);

	int		get_sockfd(void) const;

private:
	int			_sockfd;
	int			_domain;
	int			_socket_type;
	int			_protocol;
	int			_address_type;
	sockaddr_in	_address;
	int			_portnum;
	int			_backlog;
};

#endif
