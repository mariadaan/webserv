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
		size_t						_max_size;
		bool						_auto_index;
		std::string					_redirect;

	public:
		//constructors
		Location();
		Location(std::vector<std::string> &location_body);
		~Location();

		//getters
		const std::string					&get_index() const;
		const std::string					&get_root() const;
		const std::string					&get_upload() const;
		const std::string					&get_redirect() const;
		bool								get_request_methods(const std::string &key) const;
		bool							 	get_auto_index() const;
		size_t								get_max_size() const;
		
		void 	set_max_size(size_t size);
		void	print_location_class();
};

#endif
