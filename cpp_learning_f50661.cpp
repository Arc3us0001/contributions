// This tutorial will teach you how to create a very basic web server in C++.
// We will focus on the fundamental concepts of network sockets and HTTP request handling.
// By the end of this tutorial, you'll understand how a server listens for connections,
// receives data, and sends back a simple response.
// We will be using the POSIX sockets API, which is common on Linux and macOS.
// For Windows, you would typically use Winsock, but the core concepts are similar.

#include <iostream>   // For input/output operations like printing to the console.
#include <string>     // For using the std::string class to handle text.
#include <vector>     // For using std::vector to store data, though not heavily used here.
#include <cstring>    // For C-style string manipulation functions like memset.
#include <unistd.h>   // For POSIX system calls like close(), read(), write().
#include <sys/socket.h> // For socket programming functions.
#include <netinet/in.h> // For Internet domain socket structures (like sockaddr_in).
#include <arpa/inet.h>  // For functions like inet_ntoa (convert IP address to string).

// Define the port our server will listen on.
const int PORT = 8080;

int main() {
    // 1. Create a socket.
    //    AF_INET specifies the address family (IPv4).
    //    SOCK_STREAM specifies the socket type (TCP, which is reliable and connection-oriented).
    //    0 indicates the protocol (usually TCP for SOCK_STREAM).
    //    This call returns a file descriptor for the socket.
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1) {
        // If socket creation fails, print an error message and exit.
        std::cerr << "Error: Could not create socket." << std::endl;
        return 1; // Indicate an error occurred.
    }

    // 2. Bind the socket to an address and port.
    //    We need to tell the operating system which network interface and port to listen on.
    //    sockaddr_in is a structure that holds the Internet address.
    sockaddr_in address;
    // Initialize the structure to all zeros. This is important for socket programming.
    // memset is a C function that fills a block of memory with a specified value.
    memset(&address, 0, sizeof(address));

    // Set the address family to IPv4.
    address.sin_family = AF_INET;
    // Set the port number. We use htons() to convert the port number from host byte order
    // to network byte order, which is required for network protocols.
    address.sin_port = htons(PORT);
    // Set the IP address to listen on. INADDR_ANY means listen on all available network interfaces.
    address.sin_addr.s_addr = INADDR_ANY;

    // The bind() function associates the socket (server_fd) with the address and port.
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        // If bind fails, print an error and clean up the socket.
        std::cerr << "Error: Could not bind socket to port " << PORT << std::endl;
        close(server_fd); // Close the socket before exiting.
        return 1;
    }

    // 3. Listen for incoming connections.
    //    The listen() function puts the socket in a listening state.
    //    The second argument is the backlog, which is the maximum number of pending connections
    //    that can be queued up before the server starts refusing new connections.
    //    A value of 3 is common and usually sufficient for simple servers.
    if (listen(server_fd, 3) < 0) {
        // If listen fails, print an error and clean up.
        std::cerr << "Error: Could not listen on socket." << std::endl;
        close(server_fd);
        return 1;
    }

    std::cout << "Server listening on port " << PORT << "..." << std::endl;

    // 4. Accept incoming connections.
    //    The accept() function waits for a client to connect.
    //    When a connection is established, it returns a new socket file descriptor
    //    that represents the connection with the client.
    //    The sockaddr_in structure pointed to by 'client_address' will be filled with
    //    the client's address information.
    sockaddr_in client_address;
    socklen_t client_address_len = sizeof(client_address);

    // This loop will continuously accept new connections.
    while (true) {
        // accept() blocks until a connection is made.
        int new_socket = accept(server_fd, (struct sockaddr *)&client_address, &client_address_len);
        if (new_socket < 0) {
            std::cerr << "Error: Could not accept connection." << std::endl;
            // We can choose to continue or break depending on error handling strategy.
            // For this simple example, we'll continue to accept other connections.
            continue;
        }

        // Print information about the connected client.
        // inet_ntoa converts an IPv4 address (in network byte order) to a string.
        std::cout << "Connection accepted from "
                  << inet_ntoa(client_address.sin_addr)
                  << ":" << ntohs(client_address.sin_port) << std::endl;

        // 5. Handle the client's request.
        //    We'll read data from the client socket.
        char buffer[1024] = {0}; // A buffer to store incoming data.
        // read() attempts to read up to 1024 bytes from the new_socket into the buffer.
        // It returns the number of bytes read, or -1 if an error occurred.
        ssize_t bytes_read = read(new_socket, buffer, sizeof(buffer) - 1); // Leave space for null terminator

        if (bytes_read < 0) {
            std::cerr << "Error: Failed to read from socket." << std::endl;
            close(new_socket); // Close the connection.
            continue;
        }

        // For this simple server, we'll just print what we received.
        // In a real web server, you would parse the HTTP request here.
        std::cout << "Received request:\n" << buffer << std::endl;

        // 6. Send a response back to the client.
        //    This is a very basic HTTP response.
        //    "HTTP/1.1 200 OK" is the status line.
        //    "Content-Type: text/html" tells the browser what kind of content it's receiving.
        //    "\r\n\r\n" is a required separator between headers and the body.
        std::string http_response =
            "HTTP/1.1 200 OK\r\n"
            "Content-Type: text/html\r\n"
            "\r\n"
            "<html>"
            "<body><h1>Hello from your C++ Web Server!</h1></body>"
            "</html>";

        // write() sends data to the client socket.
        // It returns the number of bytes written, or -1 if an error occurred.
        write(new_socket, http_response.c_str(), http_response.length());

        // 7. Close the client connection.
        //    After sending the response, we close the connection with the client.
        close(new_socket);
        std::cout << "Connection closed." << std::endl;
    }

    // This part is technically unreachable in our infinite loop,
    // but in a real application, you'd have logic to break the loop and clean up.
    close(server_fd); // Close the main server socket.
    return 0; // Indicate successful execution.
}

// Example Usage:
// 1. Save this code as `simple_server.cpp`.
// 2. Compile it using a C++ compiler (like g++):
//    g++ simple_server.cpp -o simple_server
// 3. Run the executable:
//    ./simple_server
// 4. Open a web browser and go to:
//    http://localhost:8080
//    You should see the "Hello from your C++ Web Server!" message.
//
// To stop the server, press Ctrl+C in the terminal where it's running.