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
	char *config_file;
	if (argc < 2)
		config_file = "root/conf/server.conf";
	else
		config_file = argv[1];
	std::vector<Config>	config_vector = parse_config(config_file);
	logger << Logger::info << "Parsed config file: " << config_file << std::endl;
	try {
		Server serverSocket(PF_INET, SOCK_STREAM, 0);
		serverSocket.set_config(config_vector[0]);
		logger << Logger::info << "Starting server on port " << config_vector[0].get_port() << std::endl;

		serverSocket.set_address();
		serverSocket.bind();
		serverSocket.listen(BACKLOG);

		logger << Logger::info << "Listening on port " << config_vector[0].get_port() << ": http://localhost:" << config_vector[0].get_port() << std::endl;

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
