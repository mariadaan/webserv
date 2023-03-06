#pragma once
#ifndef NONBLOCKINGSTREAM_HPP
# define NONBLOCKINGSTREAM_HPP

#include <string>
class EventQueue;

class NonBlockingStreamReader {
	public:
		virtual void nbsr_read_impl(std::string str, EventQueue& event_queue) = 0;
		virtual void nbsr_end() {};

		virtual ~NonBlockingStreamReader() {};
};

class NonBlockingStream {
	public:
		NonBlockingStream(int fd, NonBlockingStreamReader& reader);
		virtual ~NonBlockingStream();
		void handle_event(struct kevent& ev_rec, EventQueue& event_queue);
		void write(std::string const& str);
		void write(char* str, size_t size);
		void close();

	private:
		void _end();
		void _handle_read_event(struct kevent& ev_rec, EventQueue& event_queue);
		void _handle_write_event(struct kevent& ev_rec);
		void _handle_state();
		void _write_part();

		int _fd;
		NonBlockingStreamReader& _reader;

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
