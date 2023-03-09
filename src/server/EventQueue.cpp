#include "EventQueue.hpp"
#include "Server.hpp"
#include "Client.hpp"
#include "Logger.hpp"
#include <fcntl.h>

EventQueue::EventQueue() {
	this->_kq = kqueue();
	if (this->_kq == -1)
		throw std::runtime_error("Error creating kqueue");
}

void EventQueue::add_server(Server *server) {
	this->_servers[server->get_sockfd()] = server;
	this->add_read_event_listener(server->get_sockfd());
}

void EventQueue::close_servers(void) {
	for (std::map<int, Server*>::iterator it_server = this->_servers.begin(); it_server != this->_servers.end(); it_server++) {
		(*(it_server->second)).close();
		delete it_server->second;
	}
}

/* Used for both add server listener at the very start, and adding client events */
void EventQueue::add_read_event_listener(int sockfd) {
	struct kevent kev;

	EV_SET(&kev, sockfd, EVFILT_READ, EV_ADD, 0, 0, NULL);
	int ret = ::kevent(this->_kq, &kev, 1, NULL, 0, NULL);
	if (ret == -1) {
		::close(sockfd);
		throw std::runtime_error("Error adding event listener");
	}
	if (kev.flags & EV_ERROR) {
		::close(sockfd);
		throw std::runtime_error("Event error: " + std::string(strerror(kev.data)));
	}
}

void EventQueue::add_write_event_listener(int sockfd) {
	struct kevent kev;

	EV_SET(&kev, sockfd, EVFILT_WRITE, EV_ADD | EV_CLEAR, 0, 0, NULL);
	int ret = ::kevent(this->_kq, &kev, 1, NULL, 0, NULL);
	if (ret == -1) {
		::close(sockfd);
		throw std::runtime_error("Error adding event listener");
	}
	if (kev.flags & EV_ERROR) {
		::close(sockfd);
		throw std::runtime_error("Event error: " + std::string(strerror(kev.data)));
	}
}

void EventQueue::add_cgi_event_listener(int cgi_input_fd, int cgi_output_fd, int client_sockfd) {
	struct kevent kev;

	if (::fcntl(cgi_input_fd, F_SETFL, O_NONBLOCK))
		throw (std::runtime_error("Error setting CGI to non-blocking"));
	EV_SET(&kev, cgi_input_fd, EVFILT_WRITE, EV_ADD | EV_CLEAR, 0, 0, NULL);
	int ret = ::kevent(this->_kq, &kev, 1, NULL, 0, NULL);
	if (ret == -1) {
		throw std::runtime_error("Error adding event listener");
	}
	if (kev.flags & EV_ERROR) {
		throw std::runtime_error("Event error: " + std::string(strerror(kev.data)));
	}
	this->_cgi_to_client[cgi_input_fd] = client_sockfd;

	EV_SET(&kev, cgi_output_fd, EVFILT_READ, EV_ADD, 0, 0, NULL);
	ret = ::kevent(this->_kq, &kev, 1, NULL, 0, NULL);
	if (ret == -1) {
		throw std::runtime_error("Error adding event listener");
	}
	if (kev.flags & EV_ERROR) {
		throw std::runtime_error("Event error: " + std::string(strerror(kev.data)));
	}
	this->_cgi_to_client[cgi_output_fd] = client_sockfd;
}

void EventQueue::remove_fd(int fd) {
	if (this->_client_to_server.count(fd) > 0) {
		std::map<int, int>::iterator it = this->_cgi_to_client.find(fd);
		while (it != this->_cgi_to_client.end()) {
			this->_cgi_to_client.erase(it->first);
			it = this->_cgi_to_client.find(fd);
		}
		this->_client_to_server.erase(fd);
	}
	else if (this->_cgi_to_client.count(fd) > 0) {
		this->_cgi_to_client.erase(fd);
	}
	else {
		throw std::logic_error("EventQueue: cannot remove fd");
	}
}

void EventQueue::event_loop(void) {
	struct kevent ev_rec;

	while (true) {
		// ev_rec's attributes are filled with the event that come in on the kqueue (_kq)
		// the client fd is stored in ev_rec.ident
		int num_events = ::kevent(this->_kq, NULL, 0, &ev_rec, 1, NULL);
		if (num_events == -1)
			throw std::runtime_error("Error: kevent wait");
		if (this->_servers.count(ev_rec.ident) > 0) {
			this->accept_client_on(*(this->_servers.at(ev_rec.ident)));
		}
		else if (this->_client_to_server.count(ev_rec.ident) > 0) {
			// logger << Logger::info << "Got event on client: " << ev_rec.ident << std::endl;
			(*(this->_servers.at(this->_client_to_server[ev_rec.ident]))).get_client(ev_rec.ident).handle_event(ev_rec);
		}
		else if (this->_cgi_to_client.count(ev_rec.ident) > 0) {
			// logger << Logger::info << "Got event on cgi: " << ev_rec.ident << std::endl;
			(*(this->_servers.at(this->_client_to_server[this->_cgi_to_client[ev_rec.ident]]))).get_client(this->_cgi_to_client[ev_rec.ident]).handle_cgi_event(ev_rec);
		}
		else {
			throw std::runtime_error("Got event on unknown fd");
		}
	}
}

void EventQueue::accept_client_on(Server &server) {
	try {
		Client &client = server.accept();
		this->_client_to_server[client.get_sockfd()] = server.get_sockfd();
		logger << Logger::info << "Accepted client: " << client.get_sockfd() << " (" << client.get_ip() << ")" << std::endl;
		this->add_read_event_listener(client.get_sockfd());
		this->add_write_event_listener(client.get_sockfd());
	}
	catch (const std::exception& e) {
		logger << Logger::error << e.what() << '\n';
	}
}
