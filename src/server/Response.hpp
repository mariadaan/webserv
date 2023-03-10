#pragma once
#ifndef RESPONSE_HPP
# define RESPONSE_HPP

#include "Optional.hpp"
#include "ParsedRequest.hpp"
#include "CGI.hpp"
#include "Chunk.hpp"
#include <string>

class EventQueue;
class Client;
class Config;
class Response {
	public:
		Response(Config &config, Client &client, EventQueue& event_queue);

		void	handle_part(std::string const& received);
		bool	is_ready() const;
		bool	send();
		bool	has_error_status() const;
		void	handle_cgi_event(struct kevent& ev_rec);
		bool	exceeds_max_body_size();
		void	handle_cgi_output(std::string const& str);
		void	handle_cgi_end();
		void	make_sure_cgi_done();

	private:
		Client		&_client;
		EventQueue	&_event_queue;

		Config		&_config;
		ChunkBuilder	_chunk_builder;
		Optional<ParsedRequest>	_request;
		Optional<CGI>	_cgi;
		enum {
			PARSING,
			READY,
			SENT
		} _ready;

		HTTP_STATUS_CODES	_status_code;

		size_t	_body_size;

		std::string _cgi_buffer;

		void	_mark_ready(void);

		bool	_headers_parsed(void) const;
		bool	_go_to_cgi(void) const;

		void	_build_request(std::string const &received);
		void	_handle_request();
		void	_handle_body_chunks(std::string const &received);
		void	_handle_body(std::string const &received);
		void	_send_error_response();
		void	_send_status();
		void	_send_status(HTTP_STATUS_CODES status_code);

		void	_send_file_response();

		std::string &_get_body();

		std::string _get_status();
};

#endif
