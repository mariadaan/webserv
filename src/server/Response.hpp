#pragma once
#ifndef RESPONSE_HPP
# define RESPONSE_HPP

#include "Optional.hpp"
#include "ParsedRequest.hpp"
#include "CGI.hpp"
#include "Chunk.hpp"
#include <string>

class Client;
class Config;
class Response {
	public:
		Response(Config &config, Client &client);

		void	handle_part(std::string const &received);
		bool	is_ready() const;
		void	send();
		bool	has_error_status() const;

	private:
		Client		&_client;

		Config		&_config;
		ChunkBuilder	_chunk_builder;
		Optional<ParsedRequest>	_request;
		Optional<CGI>	_cgi;
		bool _ready;

		size_t		_body_size;

		HTTP_STATUS_CODES	_status_code;

		bool		_headers_parsed(void) const;
		bool		_go_to_cgi(void) const;

		void		_build_request(std::string const &received);
		void		_handle_request();
		void		_handle_body_chunks(std::string const &received);
		void		_handle_body(std::string const &received);
		void		_send_error_response();
		void		_send_status();
		void		_send_status(HTTP_STATUS_CODES status_code);

		void		_send_file_response();
		void		_wait_for_cgi();

		std::string &_get_body();

		std::string _get_status();
};

#endif
