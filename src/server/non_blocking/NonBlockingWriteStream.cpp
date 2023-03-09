#include "NonBlockingWriteStream.hpp"
#include "EventQueue.hpp"

NonBlockingWriteStream::NonBlockingWriteStream(int fd, NonBlockingStreamWriter& writer)
	: _fd(fd)
	, _writer(&writer)
	, _want_to_write(false)
	, _write_state(CANNOT_WRITE)
	, _close_state(OPEN)
	, _write_buffer("")
{
}

void NonBlockingWriteStream::handle_event(struct kevent& ev_rec) {
	if (ev_rec.filter != EVFILT_WRITE) {
		throw std::runtime_error("NonBlockingWriteStream: Unknown event filter");
	}
	if (ev_rec.flags & EV_EOF) {
		this->_write_state = WRITE_EOF;
		this->close();
		this->_handle_state();
		return ;
	}
	this->_write_state = CAN_WRITE;
	this->_handle_state();
}

void NonBlockingWriteStream::write(std::string const& str) {
	if (this->_close_state != OPEN) {
		throw std::logic_error("NonBlockingWriteStream: Trying to write to a closed stream!!!");
	}
	this->_want_to_write = true;
	this->_write_buffer += str;
	this->_handle_state();
}

void NonBlockingWriteStream::write(char* str, size_t size) {
	return this->write(std::string(str, size));
}

void NonBlockingWriteStream::_end() {
	this->_want_to_write = false;
	this->_close_state = CLOSED;
	this->_writer->_write_end();
}

void NonBlockingWriteStream::close() {
	this->_close_state = WANT_TO_CLOSE;
	this->_handle_state();
}

void NonBlockingWriteStream::_handle_state() {
	if (this->_write_state == WRITE_EOF && this->_close_state != CLOSED) {
		this->_end();
		return ;
	}

	if (this->_want_to_write) {
		if (this->_write_state != CAN_WRITE)
			return ;
		if (this->_close_state == CLOSED) {
			throw std::logic_error("NonBlockingWriteStream: Trying to write to a closed stream");
		}
		this->_write_part();
	}
	else {
		if (this->_close_state == WANT_TO_CLOSE) {
			this->_end();
		}
	}
}

void NonBlockingWriteStream::_write_part() {
	size_t size = this->_write_buffer.size();
	ssize_t bytes_written = ::write(this->_fd, this->_write_buffer.c_str(), size);
	if (bytes_written < 0) {
		throw std::runtime_error("NonBlockingWriteStream: Error occurred while writing");
	}
	this->_write_state = CANNOT_WRITE;
	if ((size_t)bytes_written == size) {
		this->_write_buffer = "";
		this->_want_to_write = false;
		this->_handle_state();
		return ;
	}
	this->_write_buffer = this->_write_buffer.substr(bytes_written);
	this->_want_to_write = true;
}
