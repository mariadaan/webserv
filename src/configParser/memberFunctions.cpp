#include "../configParser.hpp"

//                 **********************************************
//                 **************** CONFIG CLASS ****************
//                 **********************************************


// -------------------------------- CONSTRUCTORS ---------------------------------


//Default constructor
Config::Config()
{
	_port = 80;
	_maxSize = 1;
	_errorPage = returnDefaultErrormap();
}

//No dynamically allocated memmory in the class so destructor is empty
Config::~Config(){}

//(Deep copy only matters with pointers and references, 
//...since then the pointer gets coppied and points to the same block of memmory.
//...If the other class destructs, you are left with a pointer pointing to a freed block of memmory)
Config::Config(const Config &other)
{
	this->_port = other._port;
	this->_maxSize = other._maxSize;
	this->_serverNames = other._serverNames;
	this->_root = other._root;
	this->_cgi = other._cgi;
	this->_locations = other._locations;
	this->_errorPage = other._errorPage; 
}

//Copy assignent operator using the copy and swap idiom
//Note that the object 'other' is given as value instead of reference
//Therefore the object gets copied and it does not touch the original object
//The copy and swap idiom has multiple benifits (look up if interested)
Config& Config::operator=(Config other)
{
	std::swap(*this, other);
	return *this;
}

Config::Config(std::vector<std::string> &serverVector)
	: _port(80), _maxSize(1), _errorPage(returnDefaultErrormap())
{
	size_t	enumValue;

	for (size_t i = 0; i < serverVector.size(); i++)
	{
		std::string word = findFirstWord(serverVector[i]);
		enumValue = determineIfKeyword(word);
		if (enumValue == 0)
			continue ;
		else if (enumValue == LOCATION)
		{
			std::string	locationName = getSecondWord(serverVector[i]);
			if (locationName.empty())
				continue ;
			size_t end = findClosingBracket(serverVector, i);
			std::vector<std::string> locationBody = returnLocationBody(serverVector, i, end);
			Location location(locationBody);
			this->setLocations(locationName, location);
			while (i < end)
				i++;
		}
		else
			callKeywordFunction(enumValue, serverVector[i]);
	}
}


// ---------------------------------- SETTERS -----------------------------------------


void	Config::setPort(unsigned int &port) 								{_port = port;}
void	Config::setMaxSize(unsigned int &maxSize) 							{_maxSize = maxSize;}
void	Config::setServerNames(std::vector<std::string> &serverNames)		{_serverNames = serverNames;}
void	Config::setRoot(std::string &root) 									{_root = root;}
void	Config::setCgi(std::string &cgi) 									{_cgi = cgi;}
void	Config::setLocations(const std::string &key, const Location &value) {_locations[key] = value;}
void	Config::setErrorPage(const int &key, const std::string &value)
{
	if (_errorPage.count(key))
		_errorPage[key] = value;
	else
		std::cerr << "No error code: " << key << std::endl; 													//throw ?
}



// ---------------------------------- GETTERS -------------------------------------------


const unsigned int							&Config::getPort() const 		{return (_port);}
const unsigned int							&Config::getMaxSize() const 	{return (_maxSize);}
const std::vector<std::string>				&Config::getServerNames() const {return (_serverNames);}
const std::string							&Config::getRoot() const 		{return (_root);}
const std::string							&Config::getCgi() const 		{return (_cgi);}
// const std::map<int,std::string>			&Config::getErrorPage() const 	{return (_errorPage);}
const Location								&Config::getLocation(const std::string& key) const 
{
	 std::map<std::string,Location>::const_iterator it = _locations.find(key);
	if (it == _locations.end())
		std::cerr << "Location: '" << key << "' not found" << std::endl; 										//throw gaan gebruiken!
		// throw std::out_of_range("Location not found");
	return (it->second);
}
const std::string							&Config::getErrorPage(const int &key) const
{
	std::map<int,std::string>::const_iterator it = _errorPage.find(key);
	if (it == _errorPage.end())
		std::cerr << "Error code '" << key << "' not found" << std::endl;										//throw gaan gebruiken!
		//throw
	return (it->second);
}

// -------------------------------- OTHER FUNCTIONS -------------------------------------



void	Config::callKeywordFunction(size_t &enumValue, std::string &line)
{

	if (enumValue == LISTEN || enumValue == MAX_SIZE)
		valueToUnsigned(*this, line, enumValue);
	else if (enumValue == ROOT || enumValue == CGI)
		valueToString(*this, line, enumValue);
	else if (enumValue == SERVER_NAME)
		valueToStringVector(*this, line);
	else if (enumValue == ERROR_PAGE)
		valueToError(*this, line);
}


//prints out a Config class
void	Config::printConfigClass(void)
{
	std::cout << "address : " << this->getPort() << std::endl << std::endl;
	
	std::vector<std::string> serverNames = this->getServerNames();
	std::cout << "server name(s) :" << std::endl;
	for (size_t i = 0; i < serverNames.size(); i++)
		std::cout << serverNames[i] << std::endl;
	std::cout << std::endl;
	std::cout << "root : " << this->getRoot() << std::endl << std::endl;
	std::cout << "max_size : " << this->getMaxSize() << std::endl << std::endl;
	std::cout << "cgi : " << this->getCgi() << std::endl << std::endl;
	for (std::map<std::string, Location>::const_iterator it = _locations.begin(); it != _locations.end(); it++)
	{
		std::cout << "Location : " << it->first << std::endl;
		Location object = it->second;
		object.printLocationClass();
		std::cout << std::endl;
	}
	std::cout << "\nError page :" << std::endl;
	for (std::map<int, std::string>::const_iterator it = _errorPage.begin(); it != _errorPage.end(); it++)
		std::cout << it->first << " : " << it->second << std::endl;
	std::cout << std::endl;
}


//                       **********************************************
//                       ************** LOCATION CLASS ****************
//                       **********************************************


// ------------------------------------- CONSTRUCTORS ----------------------------------------


Location::~Location(){}

Location::Location()
	: _requestMethods(returnFalseMethodsMap()), _autoIndex(false)
{
}

Location::Location(std::vector<std::string> &locationBody)
	: _requestMethods(returnFalseMethodsMap()), _autoIndex(false)
{
	for (size_t i = 0; i < locationBody.size(); i++)
	{
		std::string word = findFirstWord(locationBody[i]);
		if (word.empty())
			continue;
		else if (word == "autoindex"){
			std::string second = getSecondWord(locationBody[i]);
			if (second == "ON")
				_autoIndex = true;
		}
		else if (word == "upload" || word == "index"){
			std::string second = getSecondWord(locationBody[i]);
			if (second.empty())
				continue ;
			if (word == "upload")
				_upload = second;
			else if (word == "index")
				_index = second;
		}
		else if (word == "request_method")
		{
			std::stringstream	ss(locationBody[i]);
			std::string			name;
			ss >> name;
			while (ss >> name)
			{
				if (_requestMethods.count(name))
					_requestMethods[name] = true;
			}
		}
	}
}


// ----------------------------------------- GETTERS ----------------------------------------------

const std::string					&Location::getIndex() const 			{return (_index);}
const std::string					&Location::getUpload() const 			{return (_upload);}
bool							 	Location::getAutoIndex() const 			{return (_autoIndex);}
bool								Location::getRequestMethods(const std::string &key) const
{
	if (_requestMethods.count(key)) // returns 1 if the key is found otherwise 0 is returned
		return (_requestMethods.at(key)); //if the key is found the value at that key is returned
	return (false);
}

// -------------------------------------- OTHER FUNCTIONS -----------------------------------------

//prints out a Location class
void	Location::printLocationClass(void)
{
	std::cout << "index : " << this->getIndex() << std::endl;
	if (this->getAutoIndex())
		std::cout << "autoindex : on" << std::endl;
	else
		std::cout << "autoindex : off" << std::endl;

	for (std::map<std::string,bool>::const_iterator it = _requestMethods.begin(); it != _requestMethods.end(); it++)
		std::cout << it->first << " : " << it->second << std::endl;
	// std::cout << "GET : " << this->getRequestMethods("GET") << std::endl;
	// std::cout << "POST : " << _requestMethods["POST"] << std::endl;
	// std::cout << "DELETE : " << _requestMethods["DELETE"] << std::endl;
	// std::cout << "PUT : " << _requestMethods["PUT"] << std::endl;
	// std::cout << "PATCH : " << _requestMethods["PATCH"] << std::endl;
	std::cout << "upload : " << this->getUpload() << std::endl;
}
