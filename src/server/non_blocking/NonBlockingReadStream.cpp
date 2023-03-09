#include "NonBlockingReadStream.hpp"
#include "EventQueue.hpp"

NonBlockingReadStream::NonBlockingReadStream(int fd, NonBlockingStreamReader& reader)
	: _fd(fd)
	, _reader(&reader)
{
}

void NonBlockingReadStream::_end() {
	this->_reader->_read_end();
}

void NonBlockingReadStream::handle_event(struct kevent& ev_rec, EventQueue& event_queue) {
	if (ev_rec.filter != EVFILT_READ) {
		throw std::runtime_error("NonBlockingReadStream: Unknown event filter");
	}
	size_t bytes_available = ev_rec.data;
	if (bytes_available != 0) {
		char* buffer = new char[bytes_available];
		int	bytes_read = ::read(this->_fd, buffer, bytes_available);
		std::string received(buffer, bytes_read);
		delete[] buffer;
		this->_reader->_read_data(received, event_queue);
	}
	if (ev_rec.flags & EV_EOF) {
		this->_end();
	}
}
