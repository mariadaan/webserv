#include "EventQueue.hpp"

EventQueue::EventQueue(int server_sockfd) : _server_sockfd(server_sockfd) {
	this->_kq = kqueue();
	if (_kq == -1)
		throw std::runtime_error("Error creating kqueue");
}

void EventQueue::add_server_listener(void)
{
	struct kevent kev;

	EV_SET(&kev, _server_sockfd, EVFILT_READ, EV_ADD, 0, 0, NULL);
	int ret = kevent(_kq, &kev, 1, NULL, 0, NULL);
	if (ret == -1) {
		close(_server_sockfd);
		throw std::runtime_error("Error adding event listener");
	}
	if (kev.flags & EV_ERROR) {
		close(_server_sockfd);
		throw std::runtime_error("Event error: " + std::string(strerror(kev.data)));
	}
}

void EventQueue::wait_for_events(void)
{
	struct kevent ev_rec;

	while (true) {
		int ret = kevent(_kq, NULL, 0, &ev_rec, 1, NULL);
		if (ret == -1) {
			std::cerr << "Error: kevent wait" << std::endl;
			return ;
		}

		if (ev_rec.ident == _server_sockfd) {
			if (accept_client(_server_sockfd, _kq) != 0)
				return ;
		}
		else {
			if (handle_client(ev_rec.ident, _kq) != 0)
				return ;
		}
	}
}

int accept_client(int sockfd, int kq) {
	sockaddr_in client_address;
	socklen_t client_address_len = sizeof(client_address);
	int client_sockfd;

	client_sockfd = accept(sockfd, (sockaddr*) &client_address, &client_address_len);
	if (client_sockfd <= 0) {
		std::cerr << "Error: accept" << std::endl;
		return -1;
	}

	struct kevent kev;
	EV_SET(&kev, client_sockfd, EVFILT_READ, EV_ADD, 0, 0, NULL); // don't know args 5, 6, 7
	int ret = kevent(kq, &kev, 1, NULL, 0, NULL); // don't know args 3, 5, 6
	if (ret == -1) {
		std::cerr << "Error adding event listener	" << std::endl;
		return -1;
	}
	if (kev.flags & EV_ERROR) {
		std::cerr << "Event error: " << strerror(kev.data) << std::endl;
		return -1;
	}

	std::cerr << "Accepted client: " << client_sockfd << std::endl;
	return 0;
}

int remove_client(int kq, int client_sockfd) {
	struct kevent kev;
	EV_SET(&kev, client_sockfd, EVFILT_READ, EV_DELETE, 0, 0, NULL); // don't know args 5, 6, 7
	int ret = kevent(kq, &kev, 1, NULL, 0, NULL); // don't know args 3, 5, 6
	if (ret == -1) {
		std::cerr << "Error removing event listener" << std::endl;
		return -1;
	}
	if (kev.flags & EV_ERROR) {
		std::cerr << "Event error: " << strerror(kev.data) << std::endl;
		return -1;
	}
	close(client_sockfd);
	return 0;
}

int handle_client(int client_sockfd, int kq) {
	char buffer[1024];
	int bytes_read = read(client_sockfd, buffer, sizeof(buffer));
	std::string request(buffer, bytes_read);

	std::cout << "Received request:" << std::endl;
	std::cout << request << std::endl;

	// Send the response
	std::string response = "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\nContent-Length: 13\r\n\r\nHello, world!";
	write(client_sockfd, response.c_str(), response.size());

	if (remove_client(kq, client_sockfd) != 0) // NOTE: Do not do this with chunked requests
		return -1;

	return 0;
}