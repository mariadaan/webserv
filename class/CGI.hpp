#ifndef CGI_HPP
# define CGI_HPP

#include "ParsedRequest.hpp"
#include "Client.hpp"
#include <map>
#include <vector>

class CGI {
public:
	CGI(ParsedRequest const& request, Client& client);
	void serve();

private:
	Client&	_client;
	std::map<std::string, std::string> _env;

	std::vector<char *> get_argv() const;
	std::vector<char *> get_envp() const;
};

#endif
