#include "Logger.hpp"

Logger &Logger::filter(Flag flag) {
	this->_filter = flag;
	return *this;
}

Logger &Logger::operator<<(Flag flag) {
	this->_flag = flag;
	switch (this->_flag) {
		case Logger::debug:
			this->_ios << "\x1b[32m" << "DEBUG:   " << "\x1b[0m";
			break;
		case Logger::info:
			this->_ios << "\x1b[36m" << "INFO:    " << "\x1b[0m";
			break;
		case Logger::warn:
			this->_ios << "\x1b[33m" << "WARNING: " << "\x1b[0m";
			break;
		case Logger::error:
			this->_ios << "\x1b[31m" << "ERROR:   " << "\x1b[0m";
			break;
	}
	return *this;
}

Logger &Logger::operator<<(std::ostream &(*f)(std::ostream &)) {
	if (this->_filter > this->_flag)
		return *this;
	this->_ios << f;
	return *this;
}
