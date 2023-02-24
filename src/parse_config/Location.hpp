#pragma once
#ifndef LOCATION_HPP
# define LOCATION_HPP
#include <map>
#include <iostream>
#include "Optional.hpp"

class Location
{
	private:
		std::string					_index;
		std::string					_root;
		std::map<std::string, bool>	_request_methods;
		std::string					_upload;
		unsigned int				_max_size;
		bool						_auto_index;
		Optional<std::string>		_redirect;

	public:
		//constructors
		Location();
		Location(std::vector<std::string> &location_body);
		~Location();

		//getters
		const std::string					&get_index() const;
		const std::string					&get_upload() const;
		bool								get_request_methods(const std::string &key) const;
		bool							 	get_auto_index() const;

		void	print_location_class();
};

#endif


// void Response::_build_request(std::string const &received) {
// 	if (!this->_request.is_set()) {
// 		this->_request = Optional<ParsedRequest>(ParsedRequest(received));
// 	}
// 	else {
// 		this->_request.parse_part(received);
// 	}
// }
