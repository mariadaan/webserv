#include "EventQueue.hpp"
#include "Client.hpp"

EventQueue::EventQueue(int server_sockfd) : _server_sockfd(server_sockfd) {
	this->_kq = kqueue();
	if (_kq == -1)
		throw std::runtime_error("Error creating kqueue");
}

/* Used for both add server listener at the very start, and adding client events */
void EventQueue::add_event_listener(int sockfd) {
	struct kevent kev;

	EV_SET(&kev, sockfd, EVFILT_READ, EV_ADD, 0, 0, NULL);
	int ret = kevent(_kq, &kev, 1, NULL, 0, NULL);
	if (ret == -1) {
		close(sockfd); // what happens when you try to close an fd that was never opened?
		throw std::runtime_error("Error adding event listener");
	}
	if (kev.flags & EV_ERROR) {
		close(sockfd);
		throw std::runtime_error("Event error: " + std::string(strerror(kev.data)));
	}
}

void EventQueue::event_loop(void) {
	struct kevent ev_rec;

	while (true) {
		// ev_rec's attributes are filled with the event that come in on the kqueue (_kq)
		// the client fd is stored in ev_rec.ident
		int num_events = kevent(_kq, NULL, 0, &ev_rec, 1, NULL);
		if (num_events == -1)
			throw std::runtime_error("Error: kevent wait");
		if (ev_rec.ident == _server_sockfd)
			this->accept_client(_server_sockfd);
		else
			this->handle_client(ev_rec.ident);
	}
}

void EventQueue::accept_client(int server_sockfd) {
	Client client;

	try {
		client.accept(server_sockfd);
		this->add_event_listener(client.get_sockfd());
	}
	catch(const std::exception& e) {
		std::cerr << e.what() << '\n';
	}
	std::cerr << "Accepted client: " << client.get_sockfd() << std::endl;
}

/* TODO: deze functie verbeteren, request doorsturen aan parser en evt aan CGI */
void EventQueue::handle_client(int client_sockfd) {
	char buffer[1024];
	int bytes_read = read(client_sockfd, buffer, sizeof(buffer));
	std::string request(buffer, bytes_read);

	std::cout << "Received request:" << std::endl;
	std::cout << request << std::endl;

	// Send the response
	std::string response = "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\nContent-Length: 13\r\n\r\nHello, world!";
	write(client_sockfd, response.c_str(), response.size());

	remove_client(client_sockfd); // NOTE: Do not do this with chunked requests
}

void EventQueue::remove_client(int client_sockfd) {
	struct kevent kev;

	EV_SET(&kev, client_sockfd, EVFILT_READ, EV_DELETE, 0, 0, NULL);
	int ret = kevent(this->_kq, &kev, 1, NULL, 0, NULL);
	if (ret == -1)
		throw std::runtime_error("Error removing client");
	if (kev.flags & EV_ERROR)
		throw std::runtime_error("Event error: " + std::string(strerror(kev.data)));
	close(client_sockfd);
}
