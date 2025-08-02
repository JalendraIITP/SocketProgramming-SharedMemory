#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#include <bits/stdc++.h>

#define PORT 8080

int main() {
  int sock = 0;
  struct sockaddr_in serv_addr;
  std::string hello = "Hello from client";

  // Create socket
  sock = socket(AF_INET, SOCK_STREAM, 0);
  if (sock < 0) {
    std::cerr << "Socket creation error" << std::endl;
    return -1;
  }

  serv_addr.sin_family = AF_INET;
  serv_addr.sin_port = htons(PORT);

  // Convert IPv4 and IPv6 addresses from text to binary
  if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
    std::cerr << "Invalid address/ Address not supported" << std::endl;
    return -1;
  }

  // Connect to server(blocks)
  if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
    std::cerr << "Connection Failed" << std::endl;
    return -1;
  }

  // Send data
  int cnt = 1;
  while(1){
    std::string ss = hello;
    send(sock, ss.c_str(), ss.size(), 0);
    std::cout << "Message sent" << cnt << std::endl;
    sleep(4);
    cnt += 1;
  }
  // char buffer[1024]={'0'};
  // while(read(sock, buffer, 1024)>0){
  //     std::cout << "Message from Server: " << buffer << std::endl;
  //   }

  // Close socket
  close(sock);
  return 0;
}