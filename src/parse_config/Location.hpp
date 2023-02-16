#pragma once
#ifndef LOCATION_HPP
# define LOCATION_HPP

#include <map>
#include <iostream>

class Location
{
	private:
		std::string					_index;
		std::map<std::string, bool>	_request_methods;
		std::string					_upload;
		bool						_auto_index;

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
