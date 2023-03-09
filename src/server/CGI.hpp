#pragma once
#ifndef CGI_HPP
# define CGI_HPP

#include "Optional.hpp"
#include "NonBlockingRWStream.hpp"
#include "enums.hpp"
#include <map>
#include <vector>

class Client;
class ParsedRequest;
class Response;
class CGI : private NonBlockingStreamReader, private NonBlockingStreamWriter {
public:
	CGI(ParsedRequest const& request, Response& response, Client& client, EventQueue& event_queue);
	CGI(CGI const& src);
	CGI& operator=(CGI const& src);
	void write(char const* buf, size_t count);
	void end_of_input();
	HTTP_STATUS_CODES wait();
	void handle_event(struct kevent& ev_rec);
	void make_sure_done();

protected:
	CGI() {};

private:
	int	_pipe_fd_input[2];
	int	_pipe_fd_output[2];
	std::map<std::string, std::string> _env;
	pid_t _pid;

	Response* _response;
	Client* _client;
	EventQueue* _event_queue;

	NonBlockingRWStream _stream_event_handler;

	bool _is_running;

	bool _can_receive_input;
	bool _can_give_output;

	void _start();
	std::vector<char *> _get_argv() const;
	std::vector<char *> _get_envp() const;
	void _init_env(ParsedRequest const& request, Client& client);

	int _get_input_read_fd() const;
	int _get_input_write_fd() const;
	int _get_output_read_fd() const;
	int _get_output_write_fd() const;

	virtual void _read_data(std::string str);
	virtual void _read_end();
	virtual void _write_end();
};

#endif
