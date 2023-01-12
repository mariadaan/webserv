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

	void	add_server_listener(void);
	void	wait_for_events(void);

private:
	int		_kq;
	int		_server_sockfd;
};

int accept_client(int sockfd, int kq);
int remove_client(int kq, int client_sockfd);
int handle_client(int client_sockfd, int kq);

#endif
