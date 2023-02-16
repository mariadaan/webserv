#include "Client.hpp"
#include "EventQueue.hpp"
#include "ParsedRequest.hpp"
#include "FileResponse.hpp"
#include "Chunk.hpp"
#include "util.hpp"
#include "Logger.hpp"
#include <sstream>
#include <fcntl.h>

Client::Client(Config &config, int client_sockfd, sockaddr_in client_address)
	: config(config), _client_sockfd(client_sockfd), _client_address(client_address), _response(config, *this) {
	if (fcntl(this->_client_sockfd, F_SETFL, O_NONBLOCK))
		throw (std::runtime_error("Error setting socket to non-blocking"));
}

// TODO: should we also remove it from the map in the Server class?
void Client::close(void)
{
	::close(this->_client_sockfd);
}

int Client::get_sockfd(void) const
{
	return (this->_client_sockfd);
}

std::string Client::get_ip(void) const
{
	std::stringstream ss;
	uint32_t s_addr = ntohl(this->_client_address.sin_addr.s_addr);
	ss << ((s_addr >> 24) & 0xFF) << "."
	   << ((s_addr >> 16) & 0xFF) << "."
	   << ((s_addr >> 8) & 0xFF) << "."
	   << (s_addr & 0xFF);
	return (ss.str());
}

void Client::handle_event(struct kevent &ev_rec)
{
	if (ev_rec.filter == EVFILT_READ)
	{
		if (ev_rec.flags & EV_EOF)
		{
			logger << Logger::info << "Client disconnected: " << this->get_sockfd() << std::endl;
			this->close();
		}
		else
		{
			size_t bytes_available = ev_rec.data;
			char *buffer = new char[bytes_available];
			int bytes_read = recv(this->get_sockfd(), buffer, bytes_available, 0);
			std::string received(buffer, bytes_read);
			delete[] buffer;

			this->_response.handle_part(received);
			if (this->_response.is_ready()) {
				this->_response.send();
				this->close();
			}
		}
	}
}
