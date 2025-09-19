#include<iostream>
#include<sys/socket.h>
using namespace std;
int main(){
    /*
    What are Sockets?
    Sockets act as the contact points for network data exchange, it's like endpoints for sending and receiving data across a network.
    */

    // 1. The socket() is a system call in network programming that creates a new TCP socket in C++ that is defined inside the <sys/socket.h> header file.
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    // return;
    /*
        int sockfd declares an integer variable that will store the socket file descriptor.
        AF_INET indicates the socket will use the IPv4 address family && AF_INET6(IPv6).
        SOCK_STREAM specifies that the socket will use TCP (a stream-oriented protocol) and,
        0 lets the system choose the default protocol for the specified address family and socket type (which is TCP in this case).
    */

    // 2. The bind() method is associated with a socket, with a specific local address and port number which allows the socket to listen for incoming connection on that address.
    bind(sockfd, (struct sockaddr*)&address, sizeof(address));
    /*
        sockfd is the file descriptor that represents the socket in your program and is used to perform various socket operations
        (struct sockaddr)&address casts the address structure to a generic pointer type for the bind function.
        sizeof(address) specifies the size of the address structure to inform the system how much data to expect.
    */
    
    // 3. The listen() function marks the socket as a passive socket which prepares a socket to accept incoming connection requests (for servers).
    listen(sockfd, 10);
    /*
        sockfd is the file descriptor that represents the socket in your program and is used to perform various socket operations
        10 is the backlog parameter, which specifies the maximum number of pending connections that can be queued while the server is busy.
    */

    // 4. The accept() function accepts a new connection from a client (for servers). It extracts the first connection request on the queue of pending connections and creates a new socket for that connection.
    int clientSocket = accept(sockfd, (struct sockaddr*)&clientAddress, &clientLen);
    /*
        sockfd: It's the socket's file descriptor where it is used to perform various socket operations.
        (struct sockaddr)&address: This is a type cast that converts the pointer type of clientAddress to a pointer of type struct sockaddr*.
        &clientLen: It is a pointer to a variable that holds the size of the clientAddress.
    */
   
    return 0;
}