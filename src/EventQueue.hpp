#ifndef EVENTQUEUE_HPP
 #define EVENTQUEUE_HPP

#include "Socket.hpp"

#include <sys/types.h>
#include <sys/event.h>
#include <iostream>
#include <unistd.h>

#include <sys/socket.h>
#include <netinet/in.h>


class EventQueue
{
public:
	EventQueue(Socket &server);

	void	add_event_listener(int sockfd);
	void	event_loop(void);

	void	accept_client();
	void	handle_client(Client &client);
	void	remove_client(Client &client);

private:
	int		_kq;
	Socket	&_server;
};


#endif