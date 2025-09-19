#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <sys/select.h>
#include <fcntl.h>
#include <iostream>
#include <vector>
#include <cstring>

#define PORT 8080

int main() {
    int server_fd, new_socket;
    struct sockaddr_in address{};
    int opt = 1;
    int addrlen = sizeof(address);

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, 10) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    // Keep track of client sockets
    std::vector<int> clients;

    fd_set readfds;
    int max_fd = server_fd;

    while (true) {
        FD_ZERO(&readfds);
        FD_SET(server_fd, &readfds);

        for (int sock : clients) {
            FD_SET(sock, &readfds);
            if (sock > max_fd) max_fd = sock;
        }

        // Block until some socket is ready
        int activity = select(max_fd + 1, &readfds, nullptr, nullptr, nullptr);
        if (activity < 0) {
            perror("select error");
            break;
        }

        // New connection
        if (FD_ISSET(server_fd, &readfds)) {
            new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen);
            if (new_socket < 0) {
                perror("accept");
                continue;
            }
            clients.push_back(new_socket);
            std::cout << "New client connected: " << new_socket << std::endl;
        }

        // Handle client messages
        char buffer[1024];
        for (auto it = clients.begin(); it != clients.end();) {
            int sock = *it;
            if (FD_ISSET(sock, &readfds)) {
                int valread = read(sock, buffer, sizeof(buffer));
                if (valread <= 0) {
                    // Client disconnected
                    close(sock);
                    std::cout << "Client disconnected: " << sock << std::endl;
                    it = clients.erase(it);
                    continue;
                } else {
                    std::string msg(buffer, valread);
                    std::string message = "Message from " + std::to_string(sock) + " -> " + msg;
                    std::cout << message << std::endl;
                    for (int other : clients) {
                        if (other != sock) {
                            send(other, message.c_str(), message.size(), 0);
                        }
                    }
                }
            }
            ++it;
        }
    }

    close(server_fd);
    return 0;
}
