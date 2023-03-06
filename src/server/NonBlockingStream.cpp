#include "NonBlockingStream.hpp"
#include "EventQueue.hpp"

NonBlockingStream::NonBlockingStream(int fd, NonBlockingStreamReader& reader)
	: _fd(fd)
	, _reader(reader)
	, _want_to_write(false)
	, _write_state(CANNOT_WRITE)
	, _close_state(OPEN)
	, _write_buffer("")
{
}

NonBlockingStream::~NonBlockingStream() {
}

void NonBlockingStream::handle_event(struct kevent& ev_rec, EventQueue& event_queue) {
	if (ev_rec.filter == EVFILT_READ) {
		this->_handle_read_event(ev_rec, event_queue);
	}
	else if (ev_rec.filter == EVFILT_WRITE) {
		this->_handle_write_event(ev_rec);
	}
	else {
		throw std::runtime_error("NonBlockingStream: Unknown event filter");
	}
}

void NonBlockingStream::write(std::string const& str) {
	if (this->_close_state != OPEN) {
		throw std::logic_error("NonBlockingStream: Trying to write to a closed stream!!!");
	}
	this->_want_to_write = true;
	this->_write_buffer += str;
	this->_handle_state();
}

void NonBlockingStream::write(char* str, size_t size) {
	return this->write(std::string(str, size));
}

void NonBlockingStream::_end() {
	::close(this->_fd);
	this->_want_to_write = false;
	this->_close_state = CLOSED;
	this->_reader.nbsr_end();
}

void NonBlockingStream::close() {
	this->_close_state = WANT_TO_CLOSE;
	this->_handle_state();
}

void NonBlockingStream::_handle_read_event(struct kevent& ev_rec, EventQueue& event_queue) {
	if (ev_rec.flags & EV_EOF) {
		this->_end();
		return ;
	}
	size_t bytes_available = ev_rec.data;
	char* buffer = new char[bytes_available];
	int	bytes_read = ::read(this->_fd, buffer, bytes_available);
	std::string received(buffer, bytes_read);
	delete[] buffer;
	this->_reader.nbsr_read_impl(received, event_queue);
}

void NonBlockingStream::_handle_write_event(struct kevent& ev_rec) {
	if (ev_rec.flags & EV_EOF) {
		this->_write_state = WRITE_EOF;
		this->close();
		this->_handle_state();
		return ;
	}
	this->_write_state = CAN_WRITE;
	this->_handle_state();
}

void NonBlockingStream::_handle_state() {
	if (this->_write_state == WRITE_EOF && this->_close_state != CLOSED) {
		this->_end();
		return ;
	}

	if (this->_want_to_write) {
		if (this->_write_state != CAN_WRITE)
			return ;
		if (this->_close_state == CLOSED) {
			throw std::logic_error("NonBlockingStream: Trying to write to a closed stream");
		}
		this->_write_part();
	}
	else {
		if (this->_close_state == WANT_TO_CLOSE) {
			this->_end();
		}
	}
}

void NonBlockingStream::_write_part() {
	size_t size = this->_write_buffer.size();
	ssize_t size_written = ::write(this->_fd, this->_write_buffer.c_str(), size);
	if (size_written == -1) {
		throw std::runtime_error("Error writing to socket");
	}
	this->_write_state = CANNOT_WRITE;
	if ((size_t)size_written == size) {
		this->_write_buffer = "";
		this->_want_to_write = false;
		return ;
	}
	this->_write_buffer = this->_write_buffer.substr(size_written);
	this->_want_to_write = true;
}
