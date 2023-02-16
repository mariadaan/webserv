#pragma once
#ifndef LOGGER_HPP
# define LOGGER_HPP

#include <string>
#include <iostream>
#include "Optional.hpp"

class Logger {
public:
	Logger() : _ios(std::cerr) {};
	Logger(std::ostream &ios) : _ios(ios) {};
	// Logger(std::ostream &ios) : _ios(ios) {};

	enum Flag {
		debug,
		info,
		warn,
		error
	};

	Logger &filter(Flag flag);

	Logger &operator<<(Flag flag);

	template <typename T>
	Logger &operator<<(T const &t) {
		if (this->_filter > this->_flag)
			return *this;
		this->_ios << t;
		return *this;
	};

	Logger &operator<<(std::ostream &(*f)(std::ostream &));

private:
	std::ostream &_ios;
	Flag _flag;
	Flag _filter;
};

extern Logger logger;

#endif
