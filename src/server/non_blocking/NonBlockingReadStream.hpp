#pragma once
#ifndef NONBLOCKINGREADSTREAM_HPP
# define NONBLOCKINGREADSTREAM_HPP

#include <string>
class EventQueue;

class NonBlockingStreamReader {
	public:
		virtual void _read_data(std::string str) = 0;
		virtual void _read_end() = 0;

		virtual ~NonBlockingStreamReader() {};
};

class NonBlockingReadStream {
	public:
		NonBlockingReadStream(int fd, NonBlockingStreamReader& reader);
		virtual ~NonBlockingReadStream() {};
		void handle_event(struct kevent& ev_rec);
		NonBlockingReadStream() {};

	private:
		void _end();

		int _fd;
		NonBlockingStreamReader* _reader;
};

#endif
