#include <iostream>
#include <string>
#include <cstring>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/event.h> // kqueue

int sockfd;

void handle(int s) {
	close(sockfd);
	exit(1);
}

int PORT = 8080;
const int BACKLOG = 5;
#include <sstream>

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

int add_server_listener(int kq, int sockfd) {
	// Add a listener to the kqueue
	struct kevent kev;
	EV_SET(&kev, sockfd, EVFILT_READ, EV_ADD, 0, 0, NULL); // don't know args 3, 4, 5, 6, 7
	int ret = kevent(kq, &kev, 1, NULL, 0, NULL); // don't know args 3, 5, 6
	if (ret == -1) {
		std::cerr << "Error adding event listener	" << std::endl;
		return -1;
	}
	if (kev.flags & EV_ERROR) {
		std::cerr << "Event error: " << strerror(kev.data) << std::endl;
		return -1;
	}
	return 0;
}

void loop(int kq, int sockfd) {
	struct kevent ev_rec;

	while (true) {
		int ret = kevent(kq, NULL, 0, &ev_rec, 1, NULL);
		if (ret == -1) {
			std::cerr << "Error: kevent wait" << std::endl;
			return ;
		}

		if (ev_rec.ident == sockfd) {
			if (accept_client(sockfd, kq) != 0)
				return ;
		}
		else {
			if (handle_client(ev_rec.ident, kq) != 0)
				return ;
		}
	}
}

int main(int argc, char *argv[]) {
	signal(SIGINT, handle);

	if (argc > 1) {
		std::stringstream ss;
		std::string s(argv[1]);
		ss << s;
		ss >> PORT;
	}
	// Create a socket
	sockfd = socket(AF_INET, SOCK_STREAM, 0); // actually just a file descriptor that we will read (but we will use recv) and write (we will use send) to
	if (sockfd < 0) {
		std::cerr << "Error creating socket" << std::endl;
		return 1;
	}

	// Bind the socket to an address and port
	sockaddr_in address;
	std::memset(&address, 0, sizeof(address)); // pad the structure to the length of a struct sockaddr. set to zero
	address.sin_family = AF_INET; // IPv4
	address.sin_addr.s_addr = INADDR_ANY; // local address
	address.sin_port = htons(PORT); // convert to network byte order

	// the bind function is used to bind the socket sockfd to address and port 8080. This allows the socket to send and receive data on the specified port.
	if (bind(sockfd, (sockaddr*) &address, sizeof(address)) < 0) {
		std::cerr << "Error binding socket" << std::endl;
		return 1;
	}

	// Start listening for incoming connections
	// the listen function is used to place the socket sockfd in a listening state, with a backlog queue size of 5. This allows the socket to accept incoming connections from client sockets.
	if (listen(sockfd, BACKLOG) < 0) {
		std::cerr << "Error listening on socket" << std::endl;
		return 1;
	}

	std::cout << "Listening on port " << PORT << std::endl;

	// Create a kqueue
	int kq = kqueue();
	if (kq == -1) {
		std::cerr << "Error creating kqueue" << std::endl;
		return 1;
	}

	if (add_server_listener(kq, sockfd) != 0)
	{
		close(sockfd);
		return 1;
	}

	loop(kq, sockfd);

	// Close the socket
	close(sockfd);

	return 0;
}
