#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>

int main(int argc, char **argv)
{
    int sock = 0, valread;
    struct sockaddr_in serv_addr;
    char buffer[4096];
    char getbuf[4096];
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("\n Socket creation error \n");
        return -1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(54000);

    // Convert IPv4 and IPv6 addresses from text to binary form
    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0)
    {
        printf("\nInvalid address/ Address not supported \n");
        return -1;
    }

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        printf("\nConnection Failed \n");
        return -1;
    }
    int readVal;
    while(42)
    {
        memset(buffer, 0, 4096);
        memset(getbuf, 0, 4096);
        readVal = read(0, buffer, 4096);
        send(sock, buffer, readVal + 1, 0);
        valread = recv(sock, getbuf, 4096, 0);
        if (valread == -1)
        {
            std::cerr << "Error in recv(). Quitting" << std::endl;
            break;
        }
        if (valread == 0)
        {
            std::cout << "Client disconnected" << std::endl;
            break;
        }
        // echo message back to client
        std::cout << getbuf;
    }
    return 0;
}