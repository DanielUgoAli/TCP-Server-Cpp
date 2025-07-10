#include <iostream>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/time.h>
#include <sys/socket.h>   // Add this
#include <netinet/in.h>   // Add this
#include <arpa/inet.h>    // Add this for inet_ntoa if needed


// Server
int main(int argc, char* argv[]){

    // Check if port is provided
    if(argc != 3){
        std::cerr << "Usage: ./server port allowedRequests" << "\n";
        std::exit(0);
    }

    // Retrieve the port number
    int port = std::atoi(argv[1]);

    int numRequests = std::atoi(argv[2]);

    // Buffer for message storing 
    char buffer[2000];


    // Socket and connection tools setup
    sockaddr_in serverAddr; // variable of a struct used to define IPv4 socket address

    // So basically bzero is a function that puts a number of zero bytes in where we are pointed to
    // In this case putting  i think 3 zero-valued bytes in serveraddr area
    bzero((char*)&serverAddr, sizeof(serverAddr));

    //sin_family: specifies the address family — basically what kind of addresses this socket will use.
    // AF_INET: Stands for Address Family - Internet, 
    // and it indicates using the IPv4 protocol (as opposed to AF_INET6 for IPv6, or AF_UNIX for Unix domain sockets).
    serverAddr.sin_family = AF_INET;

    // sin_addr holds the IP address for the socket
    // s_addr holds the actual ip in binary form 
    // htonl[host of network long] converts a 32-bit value from host byte order to network byte order (big-endian)
    // INADDR_ANY is a constant of value 0.0.0.0. Basically tells the OS to bind the socket to all availableinterfaces on the host
    // ALL IN ALL  it tells the socket to bind all network interfaces to this machine and convert the address to network byte order 
    serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);

    // Then the port number 
    serverAddr.sin_port = htons(port);

    
    // server socket descriptor to be created
    // This is an integer that will store the file descriptor for the server socket
    // A socket descriptor is like a file handle that the operating system uses to identify
    // and manage the socket. When we create a socket using socket(), the OS returns
    // this descriptor, which we'll use in subsequent socket operations (bind, listen, accept)
    // The descriptor acts as a reference point for all communication through this socket
    int serverSd = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSd < 0){
        std::cerr << "Error establishing the server socket " << "\n";
        std::exit(0);
    }

    // Binding the socket to local address
    int bindStatus = bind(serverSd, (struct sockaddr*)&serverAddr, sizeof(serverAddr));
    if (bindStatus < 0){
        std::cerr << "Error binding socket to local address " << "\n";
        std::exit(0);
    }

    std::cout << " Waiting for a client to connect.... " << "\n";

    // Listen for up to a number of requests at a time
    listen(serverSd, numRequests);

    // recive a request from client using accept
    // also need a new address to connect with client

    sockaddr_in newSockAddr;
    socklen_t newSockAddrSize = sizeof(newSockAddr);

    // accept, create a new socket descriptor to handle the new connection client 
    int newSd = accept(serverSd, (sockaddr*)&newSockAddr, &newSockAddrSize);
    if(newSd < 0){
        std::cerr << "Error accepting request from client! " <<"\n";
        std::exit(1);
    }

    std::cout << "Connected with client" << "\n";

    //  Keeping track of the session (Telemetry)
    struct timeval start1, end1;
    gettimeofday(&start1, NULL);
    int bytesRead, bytesWritten = 0;
    while(1){
        // Receiving a msg from the client
        std::cout << "Awaiting client response 😎😎" << "\n";
        // Clears/zeros out the entire buffer to prepare for new data
        memset(&buffer, 0, sizeof(buffer));
        bytesRead += recv(newSd, (char*)&buffer, sizeof(buffer), 0);

        if (!strcmp(buffer, "exit")){
            std::cout << "Client has quit the session " << "\n";
            break;
        }
        std::cout << "Client: " << buffer << "\n";
        std::cout << "> ";
        std::string data;
        std::getline(std::cin, data);
        memset(&buffer, 0, sizeof(buffer));
        strcpy(buffer, data.c_str());
        if (data == "exit"){
            // send to the client that server has closed the connection
            send(newSd, (char*)&buffer, strlen(buffer), 0);
            break;
        }
        // if not send message to client 
        bytesWritten += send(newSd, (char*)&buffer, strlen(buffer), 0);

    }


    // Closing the sockets descr. after we are done
    gettimeofday(&end1, NULL);
    close(newSd);
    close(serverSd);

    std::cout << "***********Session***********" << "\n";
    std::cout << "Bytes written: " << bytesWritten << "\n" << "Bytes Read: " << bytesRead << "\n";
    std::cout << "Elapsed time " << (end1.tv_sec - start1.tv_sec) << " secs " << "\n";
    std::cout << "Connection Closed 👋" << "\n";
    return 0;
}


