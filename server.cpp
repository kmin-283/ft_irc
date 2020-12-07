#include <iostream>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string>
#include <string.h>
#include <arpa/inet.h>

#define INVALID_SOCKET 0
#define INVALID_LISTEN 0
#define SOCKET_ERROR -1

int main(int argc, char **argv)
{
    // Initialize socket //
    int server_fd, client_fd, valread;
    struct sockaddr_in server_addr;
    struct sockaddr_in client_addr;
    int opt = 1;
    char buffer[4096];
    char host[NI_MAXHOST]; // client's remote name
    char service[NI_MAXHOST]; // service (i.e port) the client is connect on

    // Create a Socket //
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET)
    {
        std::cerr << "socket failed" << std::endl;
        exit(EXIT_FAILURE);
    }
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)))
    {
        std::cerr << "setsocketopt failed" << std::endl;
        exit(EXIT_FAILURE);
    }
    // bind the ip address and port to a socket//
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(54000);
    server_addr.sin_addr.s_addr = INADDR_ANY; // could also user inet_pton ...
    
    if ((bind(server_fd, (sockaddr*)&server_addr, sizeof(server_addr))) < INVALID_SOCKET)
    {
        std::cerr << "bind failed" << std::endl;
        exit(EXIT_FAILURE);
    }
    // tell socket is for listening
    if (listen(server_fd, SOMAXCONN) < INVALID_LISTEN)
    {
        std::cerr << "lisetn failed" << std::endl;
        exit(EXIT_FAILURE);
    }
    // wait for a connection
    int clientSize = sizeof(client_addr);
    if ((client_fd = accept(server_fd, (sockaddr *)&client_addr, (socklen_t*)&clientSize)) < INVALID_SOCKET) 
    { 
        std::cerr << "accept failed" << std::endl;
        exit(EXIT_FAILURE);
    }
    memset(host, 0, NI_MAXHOST);
    memset(service, 0, NI_MAXHOST);
    if (getnameinfo((sockaddr*)&client_addr, clientSize, host, NI_MAXHOST,service, NI_MAXSERV, 0) == 0)
    {
        std::cout << host << " connected on port.. " << service << std::endl;
    }
    else
    {
        inet_ntop(AF_INET, &client_addr.sin_addr, host, NI_MAXHOST);
        std::cout << host << " connected on port... "
        << ntohs(client_addr.sin_port) << std::endl;
    }
    

    // close listening socket
    close(server_fd);

    // while loop : accept and echo message back to client
    while (42)
    {
        memset(buffer, 0, 4096);
        // wait for client to send data
        ssize_t bytesReceived = recv(client_fd, buffer, 4096, 0);
        
        if (bytesReceived == SOCKET_ERROR)
        {
            std::cerr << "Error in recv(). Quitting" << std::endl;
            break;
        }
        if (bytesReceived == 0)
        {
            std::cout << "Client disconnected" << std::endl;
            break;
        }
        // echo message back to client
        send(client_fd, buffer, bytesReceived + 1, 0);
    }
    // close the socket //    
    close(client_fd);

    // Shutdown socket //
    return (0);
}