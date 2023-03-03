# Webserv

Webserv is a 42 project where you write your own HTTP server in C++. Find the subject [here](https://cdn.intra.42.fr/pdf/pdf/69783/en.subject.pdf). 

Our webserver uses the same non-blocking, event-driven architecture [that nginx does](https://www.nginx.com/blog/inside-nginx-how-we-designed-for-performance-scale/). This allows the server to handle a high amount of requests with speed.  
We use [`kqueue`](https://www.freebsd.org/cgi/man.cgi?query=kqueue&sektion=2) to handle the multiplexing, which means the server can only run on **Mac** machines.  

## Installation

Clone the repository:

```console
git clone https://github.com/mariadaan/webserv.git
```

Compile the webserver through the makefile:

```console
make
```

Run executable (optionally with path to a configuration file):

```console
./webserver [OPTIONAL: path-to-configfile] 
```


Or run with default configuration through the makefile:

```console
make run
```

## Configuration
The server can be configured using a config file. The path-to-config file has to be added while running the server.
Possible configurations:

* Choose ports and servernames of virtual servers
* Set up default error pages
* Limit client body size
* Set up routes with following possible configurations:
  * Define accepted HTTP methods
  * Set up HTTP redirections
  * Define root directory
  * Turn on/off directory listing
  * Setup index files
  * Execute CGI based on file extensions (currently only supports Python)

Specific config file rules:
- All text that is not in a server block is not taken into account
- All text gets truncated when a ';' or '#' is encounterd
- Start of server block has to be written as "server {"
- In the server body: only the following words are handled:
	'listen', 'server_name', 'root', 'location', 'client_max_body_size', 'cgi', 'error_page'
- In the location body: the following words are handled:
'index', 'request_method', 'upload', 'autoindex', 'redirect', 'client_max_body_size', 'root'
- All other words are not taken into account
- For the 'client_max_body_size' k/K are read as kilo (*1000) and m/M as mega (*1000000)


## Resources
Visit [this Notion page](https://www.notion.so/Web-server-theory-3d234c98dc3845698dce00fb8c298080) to read more about the components of a web server.

Our webserv team:
[Marius](https://github.com/Mariusmivw)
[Pieter](https://github.com/pderksen98)
[Maria](https://github.com/mariadaan)