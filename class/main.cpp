#include "Socket.hpp"
#include "EventQueue.hpp"

#include <iostream>
#include <string>
#include <cstring>
#include <unistd.h>
#include <sstream>

int PORT = 8080;
const int BACKLOG = 5;

int main(int argc, char *argv[]) {
	if (argc > 1) {
		std::stringstream ss;
		std::string s(argv[1]);
		ss << s;
		ss >> PORT;
	}
	try
	{
		// Create a socket
		Socket serverSocket(PF_INET, SOCK_STREAM, 0);

		// Bind the socket to an address and port
		serverSocket.set_address(PORT);

		// the bind function is used to bind the socket sockfd to address and port 8080. This allows the socket to send and receive data on the specified port.
		serverSocket.bind();

		// Start listening for incoming connections
		// the listen function is used to place the socket sockfd in a listening state, with a backlog queue size of 5. This allows the socket to accept incoming connections from client sockets.
		serverSocket.listen(BACKLOG);

		std::cout << "Listening on port " << PORT << std::endl;

		// Create a kqueue for current server socket
		EventQueue keventQueue(serverSocket.get_sockfd());

		// Add listener to the queue
		keventQueue.add_server_listener();

		keventQueue.wait_for_events();

		// loop(kq, serverSocket.get_sockfd());

		// Close the socket
		serverSocket.close();

	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << '\n';
	}

	return 0;
}
