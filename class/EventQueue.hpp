#ifndef EVENTQUEUE_HPP
 #define EVENTQUEUE_HPP

#include <sys/types.h>
#include <sys/event.h>
#include <iostream>
#include <unistd.h>

#include <sys/socket.h>
#include <netinet/in.h>


class EventQueue
{
public:
	EventQueue(int server_sockfd);

	void	add_event_listener(int sockfd);
	void	event_loop(void);

	void	accept_client(int sockfd);
	void	handle_client(int client_sockfd);
	void	remove_client(int client_sockfd);

private:
	int		_kq;
	int		_server_sockfd;
};


#endif
