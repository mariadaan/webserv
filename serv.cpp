#include <iostream>
#include <string>
#include <cstring>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

const int PORT = 8080;
const int BACKLOG = 5;

int main() {
  // Create a socket
  int sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sockfd < 0) {
    std::cerr << "Error creating socket" << std::endl;
    return 1;
  }

  // Bind the socket to an address and port
  sockaddr_in address;
  std::memset(&address, 0, sizeof(address));
  address.sin_family = AF_INET;
  address.sin_addr.s_addr = INADDR_ANY;
  address.sin_port = htons(PORT);

  if (bind(sockfd, (sockaddr*) &address, sizeof(address)) < 0) {
    std::cerr << "Error binding socket" << std::endl;
    return 1;
  }

  // Start listening for incoming connections
  if (listen(sockfd, BACKLOG) < 0) {
    std::cerr << "Error listening on socket" << std::endl;
    return 1;
  }

  std::cout << "Listening on port " << PORT << std::endl;

  // Accept incoming connections
  sockaddr_in client_address;
  socklen_t client_address_len = sizeof(client_address);
  int client_sockfd;
  while ((client_sockfd = accept(sockfd, (sockaddr*) &client_address, &client_address_len)) > 0) {
    // Read the request
    char buffer[1024];
    int bytes_read = read(client_sockfd, buffer, sizeof(buffer));
    std::string request(buffer, bytes_read);

    std::cout << "Received request:" << std::endl;
    std::cout << request << std::endl;

    // Send the response
    std::string response = "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\nContent-Length: 12\r\n\r\nHello, world!";
    write(client_sockfd, response.c_str(), response.size());

    // Close the connection
    close(client_sockfd);
  }

  // Close the socket
  close(sockfd);

  return 0;
}
