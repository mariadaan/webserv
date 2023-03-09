#pragma once
#ifndef NONBLOCKINGWRITESTREAM_HPP
# define NONBLOCKINGWRITESTREAM_HPP

#include <string>

class NonBlockingStreamWriter {
	public:
		virtual void _write_end() = 0;

		virtual ~NonBlockingStreamWriter() {};
};

class NonBlockingWriteStream {
	public:
		NonBlockingWriteStream(int fd, NonBlockingStreamWriter& reader);
		virtual ~NonBlockingWriteStream() {};
		void handle_event(struct kevent& ev_rec);
		void write(std::string const& str);
		void write(char* str, size_t size);
		void close();

		NonBlockingWriteStream() {};

	private:
		void _end();
		void _handle_state();
		void _write_part();

		int _fd;
		NonBlockingStreamWriter* _writer;

		bool _want_to_write;
		enum {
			CANNOT_WRITE,
			CAN_WRITE,
			WRITE_EOF
		}	_write_state;
		enum {
			OPEN,
			WANT_TO_CLOSE,
			CLOSED
		} 	_close_state;
		std::string	_write_buffer;
};

#endif
