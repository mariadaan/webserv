#include "CGI.hpp"
#include <unistd.h>
#include <vector>
#include <cstring>

CGI::CGI(ParsedRequest const& request, Client &client) : _client(client) {
	(void)request;
	this->_env["AUTH_TYPE"] = request.get_auth_scheme();
	// this->_env["CONTENT_LENGTH"] = request.get_content_length(); // NOTE: if and only if request has a body
	// this->_env["CONTENT_TYPE"] = request.get_content_type(); // // NOTE: if and only if request has a body
	this->_env["GATEWAY_INTERFACE"] = "CGI/1.1";
	// this->_env["PATH_INFO"] = request.get_path(); // NOTE: check workings with CGI on wikipedia
	// this->_env["PATH_TRANSLATED"] = ???;
	this->_env["QUERY_STRING"] = request.get_query_string();
	this->_env["REMOTE_ADDR"] = client.get_ip(); //
	// this->_env["REMOTE_HOST"] = request.get_remote_host(); //
	this->_env["REMOTE_IDENT"] = ""; // NOTE: does not need to be implemented
	// this->_env["REMOTE_USER"] = request.get_remote_user(); //
	this->_env["REQUEST_METHOD"] = request.method_string;
	// this->_env["SCRIPT_NAME"] = ???;
	// this->_env["SERVER_NAME"] = config.get_server_name();
	// this->_env["SERVER_PORT"] = config.get_server_port();
	this->_env["SERVER_PROTOCOL"] = request.http_version;
	this->_env["SERVER_SOFTWARE"] = "Webserv/1.0";
}

std::vector<char *> CGI::get_envp() const {
	std::vector<char *> envp;

	for (decltype(this->_env)::const_iterator it = this->_env.begin(); it != this->_env.end(); ++it) {
		std::string s = it->first + "=" + it->second;
		char *ss = new char[s.size() + 1];
		std::copy(s.begin(), s.end(), ss);
		ss[s.size()] = '\0';
		envp.push_back(ss);
	}
	envp.push_back(NULL);

	return envp;
}

std::vector<char *> CGI::get_argv() const {
	std::vector<std::string> argv;
	argv.push_back("./cgi_test.py");

	std::vector<char *> converted_argv;
	for (decltype(argv)::const_iterator it = argv.begin(); it != argv.end(); ++it) {
		char *ss = new char[it->size() + 1];
		std::copy(it->begin(), it->end(), ss);
		ss[it->size()] = '\0';
		converted_argv.push_back(ss);
	}
	converted_argv.push_back(NULL);

	return converted_argv;
}

#include <sstream>
void CGI::serve() {
	pid_t cpid = ::fork();
	if (cpid == -1) {
		throw std::runtime_error("fork() failed");
	}

	if (cpid == 0) {
		if (::dup2(this->_client.get_sockfd(), STDOUT_FILENO) == -1)
			throw std::runtime_error("dup2() failed");
		::execve("./cgi_test.py", this->get_argv().data(), this->get_envp().data());
		throw std::runtime_error("execve() failed");
	}
	else {
		int status;
		::waitpid(cpid, &status, 0); // NOTE: this hangs the entire process, thus no other clients can be served
	}
}
