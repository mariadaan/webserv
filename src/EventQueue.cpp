#include "EventQueue.hpp"
#include "Server.hpp"
#include "Client.hpp"

EventQueue::EventQueue(Server &server) : _server(server) {
	this->_kq = kqueue();
	if (_kq == -1)
		throw std::runtime_error("Error creating kqueue");
}

/* Used for both add server listener at the very start, and adding client events */
void EventQueue::add_event_listener(int sockfd) {
	struct kevent kev;

	EV_SET(&kev, sockfd, EVFILT_READ, EV_ADD, 0, 0, NULL);
	int ret = kevent(this->_kq, &kev, 1, NULL, 0, NULL);
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
		int num_events = kevent(this->_kq, NULL, 0, &ev_rec, 1, NULL);
		if (num_events == -1)
			throw std::runtime_error("Error: kevent wait");
		if (ev_rec.ident == (uintptr_t)this->_server.get_sockfd())
			this->accept_client();
		else
			this->_server.get_client(ev_rec.ident).handle_event(ev_rec, *this);
	}
}

void EventQueue::accept_client() {
	try {
		Client &client = this->_server.accept();
		std::cout << "Accepted client: " << client.get_sockfd() << std::endl;
		this->add_event_listener(client.get_sockfd());
	}
	catch(const std::exception& e) {
		std::cerr << e.what() << '\n';
	}
}

void EventQueue::remove_event_listener(int sockfd) {
	struct kevent kev;

	EV_SET(&kev, sockfd, EVFILT_READ, EV_DELETE, 0, 0, NULL);
	int ret = kevent(this->_kq, &kev, 1, NULL, 0, NULL);
	if (ret == -1)
		throw std::runtime_error("Error removing client");
	if (kev.flags & EV_ERROR)
		throw std::runtime_error("Event error: " + std::string(strerror(kev.data)));
}
