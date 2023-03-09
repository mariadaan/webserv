#pragma once
#ifndef NONBLOCKINGSTREAM_HPP
# define NONBLOCKINGSTREAM_HPP

#include "NonBlockingReadStream.hpp"
#include "NonBlockingWriteStream.hpp"
#include <string>
class EventQueue;

class NonBlockingRWStream : private NonBlockingReadStream, private NonBlockingWriteStream {
	public:
		NonBlockingRWStream(int fd, NonBlockingStreamReader& reader, NonBlockingStreamWriter& writer);
		NonBlockingRWStream(int read_fd, int write_fd, NonBlockingStreamReader& reader, NonBlockingStreamWriter& writer);
		virtual ~NonBlockingRWStream() {};
		void handle_event(struct kevent& ev_rec);
		void write(std::string const& str);
		void write(char* str, size_t size);
		void close();

		NonBlockingRWStream() {};
};

#endif
