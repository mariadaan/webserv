#ifndef EVENTQUEUE_HPP
 #define EVENTQUEUE_HPP

#include <sys/types.h>
#include <sys/event.h>
#include <iostream>
#include <unistd.h>

#include <sys/socket.h>
#include <netinet/in.h>

class Server;
class EventQueue
{
public:
	EventQueue(Server &server);

	void	add_event_listener(int sockfd);
	void	event_loop(void);

	void	accept_client();
	void	remove_event_listener(int sockfd);

private:
	int		_kq;
	Server	&_server;
};


#endif
