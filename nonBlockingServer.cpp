#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include<thread>

#include <cstring>
#include <iostream>
using namespace std;
#define PORT 8080
#include<vector>

int main() {
  int server_fd, new_socket;
  struct sockaddr_in address{};
  int opt = 1;
  int addrlen = sizeof(address);
  char buffer[1024] = {0};

  server_fd = socket(AF_INET, SOCK_STREAM, 0);
  if (server_fd == 0) {
    perror("socket failed");
    exit(EXIT_FAILURE);
  }
  setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
  address.sin_family = AF_INET; 
  address.sin_addr.s_addr = INADDR_ANY;
  address.sin_port = htons(PORT);

  // Bind
  if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
    perror("bind failed");
    exit(EXIT_FAILURE);
  }

  // Listen
  if (listen(server_fd, 3) < 0) {
    perror("listen");
    exit(EXIT_FAILURE);
  }

  // Accept a connection
  vector<int>clients;
  while(1){
    new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen);
    if (new_socket < 0) {
        perror("accept");
        continue;
    }
    clients.push_back(new_socket);
    thread t([clients, new_socket](){
        char buffer[1024] = {0};

        // Read data
        while(1){
            int mlength = read(new_socket, buffer, 1024);
            if(mlength<=0)break;
            cout<<buffer<<endl;
            string str(buffer, mlength), m = to_string(new_socket);
            string message = "Message from : ";
            message += m;
            message += " -> ";
            message += str;
            for(auto sock:clients){
                if(sock != new_socket)send(sock, message.c_str(), message.size(), 0);
            }
        }
        close(new_socket);
        std::cout << "Client disconnected" << std::endl;
    });
    if(t.joinable())t.detach();
  }

  // Close socket
  close(server_fd);
  return 0;
}