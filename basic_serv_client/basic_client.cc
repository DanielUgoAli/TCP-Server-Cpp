#include <iostream>
#include <string>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <netdb.h>
#include <sys/time.h>

int main(int argc, char *argv[])
{
    // Need ip address and port number
    if (argc != 3)
    {
        std::cerr << "Usage: ./client ip_address port" << "\n";
        std::exit(1);
    }
    char *serverIp = argv[1];
    int port = atoi(argv[2]);

    char buffer[2500]; // msg buffer

    // Socket and connection tools setup
    struct hostent *host = gethostbyname(serverIp);
    if (host == NULL)
    {
        std::cerr << "Error: Unable to resolve hostname" << "\n";
        std::exit(1);
    }

    sockaddr_in sendSockAddr;
    bzero((char *)&sendSockAddr, sizeof(sendSockAddr));
    sendSockAddr.sin_family = AF_INET;
    sendSockAddr.sin_addr = *(struct in_addr *)host->h_addr_list[0];
    sendSockAddr.sin_port = htons(port);

    int clientSd = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSd < 0)
    {
        std::cerr << "Error creating client socket" << "\n";
        std::exit(1);
    }

    // TRY TO CONNECT
    int status = connect(clientSd, (sockaddr *)&sendSockAddr, sizeof(sendSockAddr));

    if (status < 0)
    {
        std::cerr << "Error connecting to socket! " << "\n";
        return -1;
    }

    std::cout << "Connected to the socket! " << "\n";
    int bytesRead, bytesWritten = 0;
    struct timeval start1, end1;
    gettimeofday(&start1, NULL);
    while (1)
    {
        std::cout << "> ";
        std::string data;
        getline(std::cin, data);
        memset(&buffer, 0, sizeof(buffer)); // clear the buffer
        strcpy(buffer, data.c_str());
        if (data == "exit")
        {
            send(clientSd, (char *)&buffer, strlen(buffer), 0);
            break;
        }
        bytesWritten += send(clientSd, (char *)&buffer, strlen(buffer), 0);
        std::cout << "Awaiting server response..." << "\n";
        memset(&buffer, 0, sizeof(buffer)); // clear the buffer
        bytesRead += recv(clientSd, (char *)&buffer, sizeof(buffer), 0);
        if (!strcmp(buffer, "exit"))
        {
            std::cout << "Server has quit the session" << "\n";
            break;
        }
        std::cout << "Server: " << buffer << "\n";
    }
    gettimeofday(&end1, NULL);
    close(clientSd);
    std::cout << "********Session********" << "\n";
    std::cout << "Bytes written: " << bytesWritten << " Bytes read: " << bytesRead << "\n";
    std::cout << "Elapsed time: " << (end1.tv_sec - start1.tv_sec) << " secs" << "\n";
    std::cout << "Connection closed" << "\n";
    return 0;
}