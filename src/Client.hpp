#ifndef CLIENT_HPP
 #define CLIENT_HPP

#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

class EventQueue;
class Client
{
public:
	Client() {}
	Client(int client_sockfd, sockaddr_in client_address, socklen_t client_address_len);
	int			get_sockfd(void) const;
	void		close(void);
	void		handle_event(struct kevent &ev_rec, EventQueue &event_queue);

private:
	int			_client_sockfd;
	sockaddr_in	_client_address;
	socklen_t	_client_address_len;
};

#endif
