#include "Server.hpp"
#include "EventQueue.hpp"
#include "Logger.hpp"
#include "configParser.hpp"

#include <iostream>
#include <string>
#include <cstring>
#include <unistd.h>
#include <sstream>

const int BACKLOG = 5;

Logger logger;

int main(int argc, char *argv[]) {
	std::vector<Config>	config_vector = parse_config("root/conf/server.conf");
	int PORT = config_vector[0].get_port();
	if (argc > 1) {
		std::stringstream ss;
		std::string s(argv[1]);
		ss << s;
		ss >> PORT;
	}
	logger << Logger::info << "Starting server on port " << PORT << std::endl;
	try {
		Server serverSocket(PF_INET, SOCK_STREAM, 0);
		serverSocket.set_address(PORT);
		serverSocket.bind();
		serverSocket.listen(BACKLOG);

		logger << Logger::info << "Listening on port " << PORT << ": http://localhost:" << PORT << std::endl;

		EventQueue keventQueue(serverSocket);
		keventQueue.add_event_listener(serverSocket.get_sockfd());
		keventQueue.event_loop();
		serverSocket.close();
	}
	catch(const std::exception& e) {
		logger << Logger::error << e.what() << std::endl;
	}

	return 0;
}
