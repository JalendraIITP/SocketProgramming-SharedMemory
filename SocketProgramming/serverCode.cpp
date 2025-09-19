#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#include <cstring>
#include <iostream>
using namespace std;
#define PORT 8080
// we also can build custom address format
class sockaddr_i {
    public:
    sa_family_t    sin_family; // Address family (AF_INET for IPv4)
    in_port_t      sin_port;   // Port number (in network byte order)
    struct in_addr sin_addr;   // IP address (in network byte order)
    char           sin_zero[8];// Padding (not used)
};
int main() {
  int server_fd, new_socket;
  struct sockaddr_in address{};// this is defined in <netinet/in.h>
  // class address:public sockaddr_in{}; sockaddr_in cant be inherited and it not a class it is written in C(struct)
  int opt = 1;
  int addrlen = sizeof(address); // This stores the size (in bytes) of the sockaddr_in structure
  char buffer[1024] = {0}; // This creates a character array of size 1024 bytes, initialized to zero, used to store incoming or outgoing data over the socket.

  // Create socket(server_fd = socket file descriptor)
  server_fd = socket(AF_INET, SOCK_STREAM, 0);
  // Use 0 unless you need a specific protocol override.
  if (server_fd == 0) {
    perror("socket failed");
    exit(EXIT_FAILURE);
  }

  // Attach socket to the port
  // int setsockopt(int sockfd, int level, int option_name,
  //              const void *option_value, socklen_t option_len);
  setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
  /*
  SOL_SOCKET: Level at which the option is defined — here it's the socket layer itself
  SO_REUSEADDR:	Allows the socket to reuse the address (IP+port), even if it’s in TIME_WAIT state
  &opt:	A pointer to the value you want to set (1 usually, meaning “enable”)
  sizeof(opt):	The size of the option value (int in this case)
  */
  address.sin_family = AF_INET; // IPv4 stands for IPv4(Internet Protocol v4)
  address.sin_addr.s_addr = INADDR_ANY; // This specifies the IP address that the server will bind to.
  /*
    Bind to all available interfaces (e.g., 127.0.0.1, LAN IP, etc.)
    So, your server listens on any local IP address.
    alternatively,
      inet_pton(AF_INET, "192.168.1.100", &address.sin_addr);
  */
  address.sin_port = htons(PORT);
  /*
   (host to network short)This sets the port number the server will listen on.
   Converts the port from host byte order (little-endian) to network byte order (big-endian).
   Network protocols always use big-endian.
  */

  // Bind
  if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
    perror("bind failed");
    exit(EXIT_FAILURE);
  }
  /*
  The bind() system call associates your server socket with:
  1. an IP address
  2. a port number
  So that it knows where to listen for incoming connections.
  */

  // Listen
  if (listen(server_fd, 3) < 0) {
    perror("listen");
    exit(EXIT_FAILURE);
  }
  /*
    int listen(int sockfd, int backlog);
    The listen() function marks the socket as passive, meaning it's ready to accept incoming client connections.
    3: This controls how many pending (not yet accepted) connections can wait in the queue.
    
    This simply tells the operating system:
      “I'm ready to accept connections on this socket.”
      “Here's how many pending connections I can queue (backlog).”
  */

  // Accept a connection
  new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen);
  if (new_socket < 0) {
    perror("accept");
    exit(EXIT_FAILURE);
  }
  /*
  Purpose of accept()
    Waits (blocks) until a client connects
    Creates a new socket (new_socket) specific to that client
    Leaves server_fd untouched so it can continue listening for others
  */

  // Read data
  while(read(new_socket, buffer, 1024)>0){
    std::cout << "Message from client: " << buffer << std::endl;
  }
  /*
  Behavior of read()
    >0 → Number of bytes read (continues the loop)
     0 → EOF (client closed connection → loop ends)
    <0 → Error (could be due to connection loss, etc.)
  */

  // Close socket
  close(new_socket);
  close(server_fd);
  return 0;
}