#include "ParsedRequest.hpp"
#include <iostream>
#include <string>
#include <cstring>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

const int PORT = 8080;
const int BACKLOG = 5;

int main() {
	// Create a socket
	int sockfd = socket(AF_INET, SOCK_STREAM, 0); // actually just a file descriptor that we will read (but we will use recv) and write (we will use send) to
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

	// Accept incoming connections
	sockaddr_in client_address;
	socklen_t client_address_len = sizeof(client_address);
	int client_sockfd;
	// the accept function is used to accept an incoming connection on the socket sockfd and create a new socket client_sockfd for the client.
	// The address of the client is stored in the client_address structure, and the address of the server is stored in the serv_addr structure.
	// The new socket client_sockfd can be used to communicate with the client.
	while ((client_sockfd = accept(sockfd, (sockaddr*) &client_address, &client_address_len)) > 0) {
		// Read the request
		char buffer[1024];
		int bytes_read = read(client_sockfd, buffer, sizeof(buffer));
		std::string request(buffer, bytes_read);

	ParsedRequest req(request);
	std::cout << req << std::endl;

		// Send the response
		std::string response = "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\nContent-Length: 13\r\n\r\nHello, world!";
		write(client_sockfd, response.c_str(), response.size());

		// Close the connection
		close(client_sockfd);
	}

	// Close the socket
	close(sockfd);

	return 0;
}
