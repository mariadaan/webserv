#include "Server.hpp"
#include "EventQueue.hpp"
#include "Logger.hpp"
#include "ParsedConfigFile.hpp"

#include <iostream>
#include <string>
#include <cstring>
#include <unistd.h>
#include <sstream>

const int BACKLOG = 5;

Logger logger;

int main(int argc, char *argv[]) {
	logger << std::boolalpha;

	std::string config_filename;

	if (argc < 2)
		config_filename = "root/conf/server.conf";
	else
		config_filename = argv[1];
	logger << Logger::info << "Parsed config file: " << config_filename << std::endl;
	try {
		ParsedConfigFile config(config_filename.c_str());
		EventQueue keventQueue;
		try {
			for (std::vector<Config>::iterator it = config.server_blocks.begin(); it < config.server_blocks.end(); it++) {
				Server *serverSocket = new Server(*it, PF_INET, SOCK_STREAM, 0);

				logger << Logger::info << "Starting server on port " << (*it).get_port() << std::endl;

				try {
					serverSocket->set_address();
					serverSocket->bind();
					serverSocket->listen(BACKLOG);
				}
				catch (const std::exception&) {
					delete serverSocket;
					throw ;
				}

				logger << Logger::info << "Listening on port " << (*it).get_port() << ": http://localhost:" << (*it).get_port() << std::endl;
				keventQueue.add_server(serverSocket);
			}
			keventQueue.event_loop();
			keventQueue.close_servers();
		}
		catch (const std::exception&) {
			keventQueue.close_servers();
			throw ;
		}
	}
	catch (const std::exception& e) {
		logger << Logger::error << e.what() << std::endl;
		return (EXIT_FAILURE);
	}
	return (EXIT_SUCCESS);
}
