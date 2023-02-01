#include <stdlib.h>
#include <string>
#include <iostream>
#include <stdexcept>
#include <fstream>
#include <iostream>
#include <vector>
#include <map>
#include <sstream> 

enum conf_parser {
	LISTEN = 1,
	SERVER_NAME,
	ROOT,
	LOCATION,
	MAX_SIZE,
	ERROR_PAGE,
	CGI
};

class Location
{
	private:
		std::string					_index;
		std::map<std::string, bool>	_requestMethods;
		std::string					_upload;
		bool						_autoIndex;

	public:
		//constructors
		Location();
		Location(std::vector<std::string> &locationBody);
		~Location();

		//getters
		const std::string					&getIndex() const;
		const std::string					&getUpload() const;
		bool								getRequestMethods(const std::string &key) const;
		bool							 	getAutoIndex() const;

		void	printLocationClass();
};

class Config
{
	private:
		unsigned int						_port;
		unsigned int						_maxSize;
		std::vector<std::string>			_serverNames;
		std::string							_root;
		std::string							_cgi;
		std::map<std::string,Location>		_locations;
		std::map<int, std::string>	_errorPage;
	
	public:
		//constructors
		Config();
		~Config();
		Config(const Config &other);							//not needed yet
		Config& operator=(Config other); //copy and swap idiom 	//not needed yet
		Config(std::vector<std::string> &serverVector);

		//getters
		const unsigned int						&getPort() const;
		const unsigned int						&getMaxSize() const;
		const std::vector<std::string>			&getServerNames() const;
		const std::string						&getRoot() const;
		const std::string						&getCgi() const;
		const Location							&getLocation(const std::string& key) const;
		const std::string						&getErrorPage(const int &key) const;	

		//setters
		void	setPort(unsigned int &port);
		void	setMaxSize(unsigned int &maxSize);
		void	setServerNames(std::vector<std::string> &serverNames);
		void	setRoot(std::string &root);
		void	setCgi(std::string &cgi);
		void	setLocations(const std::string &key, const Location &value);
		void	setErrorPage(const int &key, const std::string &value);

		void	callKeywordFunction(size_t &enumValue, std::string &line);
		void	printConfigClass();
};


//print.cpp
void									printStringVector(std::vector<std::string> &vec);
void									printServerVector(std::vector<std::vector<std::string> > &vec);
//configUtils.cpp
size_t									determineIfKeyword(const std::string &word);
std::vector<std::string>				returnLocationBody(std::vector<std::string> &serverVector, size_t i, size_t end);
unsigned int							stringToUnsigned(std::string &word);
void									valueToUnsigned(Config &object, std::string &line, size_t &enumValue);
void									valueToStringVector(Config &object, std::string &line);
void									valueToString(Config &object, std::string &line, size_t &enumValue);
void									valueToError(Config &object, std::string &line);
std::map<std::string, bool>				returnFalseMethodsMap(void);
std::map<int,std::string>				returnDefaultErrormap(void);
std::string								truncateString(const std::string &str, char c);
std::string								getThirdWord(std::string &line);
std::string								getSecondWord(std::string &line);
std::string 							findFirstWord(std::string &line) ;
bool									checkBrackets(const std::vector<std::string> &vec);
size_t									findClosingBracket(const std::vector<std::string> &vec, size_t line);
//configParser.cpp
std::vector<Config>						setConfigVector(std::vector<std::vector<std::string> > &serverVector);
std::vector<std::string>				findServerBlock(const std::vector<std::string> &vec);
std::vector<std::vector<std::string> >	createServerVector(const std::vector<std::string> &file);
const std::vector<std::string>			getFileVector(const std::string &fileName);
//parse_config.cpp
std::vector<Config>						parse_config(char const *conf_file);
