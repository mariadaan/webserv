#ifndef CONFIG_HPP
# define CONFIG_HPP

#include "Location.hpp"

#include <iostream>
#include <vector>
#include <map>

class Config
{
	private:
		unsigned int						_port;
		unsigned int						_max_size;
		std::vector<std::string>			_server_names;
		std::string							_root;
		std::string							_cgi;
		std::map<std::string,Location>		_locations;
		std::map<int, std::string>			_error_page;
	
	public:
		//constructors
		Config();
		~Config();
		Config& operator=(Config other); //copy and swap idiom 	(not needed yet)
		Config(std::vector<std::string> &server_vector);

		//getters
		const unsigned int				&get_port() const;
		const unsigned int				&get_max_size() const;
		const std::vector<std::string>	&get_server_names() const;
		const std::string				&get_root() const;
		const std::string				&get_cgi() const;
		const Location					&get_location(const std::string& key) const;
		const std::string				&get_error_page(const int &key) const;	

		//setters
		void	set_port(unsigned int &port);
		void	set_max_size(unsigned int &max_size);
		void	set_server_names(std::vector<std::string> &server_names);
		void	set_root(std::string &root);
		void	set_cgi(std::string &cgi);
		void	set_locations(const std::string &key, const Location &value);
		void	set_error_page(const int &key, const std::string &value);

		void	call_keyword_function(size_t &enumValue, std::string &line);
		void	print_config_class();
};

#endif
