#include "NonBlockingReadStream.hpp"
#include "NonBlockingRWStream.hpp"
#include "NonBlockingWriteStream.hpp"
#include "EventQueue.hpp"

NonBlockingRWStream::NonBlockingRWStream(int fd, NonBlockingStreamReader& reader, NonBlockingStreamWriter& writer)
	: NonBlockingReadStream(fd, reader)
	, NonBlockingWriteStream(fd, writer)
{
}

NonBlockingRWStream::NonBlockingRWStream(int read_fd, int write_fd, NonBlockingStreamReader& reader, NonBlockingStreamWriter& writer)
	: NonBlockingReadStream(read_fd, reader)
	, NonBlockingWriteStream(write_fd, writer)
{
}

void NonBlockingRWStream::handle_event(struct kevent& ev_rec, EventQueue& event_queue) {
	if (ev_rec.filter == EVFILT_READ) {
		NonBlockingReadStream::handle_event(ev_rec, event_queue);
	}
	else if (ev_rec.filter == EVFILT_WRITE) {
		NonBlockingWriteStream::handle_event(ev_rec);
	}
	else {
		throw std::runtime_error("NonBlockingRWStream: Unknown event filter");
	}
}

void NonBlockingRWStream::write(std::string const& str) {
	NonBlockingWriteStream::write(str);
}

void NonBlockingRWStream::write(char* str, size_t size) {
	NonBlockingWriteStream::write(str, size);
}

void NonBlockingRWStream::close() {
	NonBlockingWriteStream::close();
}
