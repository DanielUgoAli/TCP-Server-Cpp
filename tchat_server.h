#pragma once

#include <vector>
#include <mutex>
#include <string>
#include <thread>
#include <sys/socket.h>

// Other necessary so i don't have to in the implmentation
#include <unistd.h>
#include <cstring>
#include <iostream>
#include <netinet/in.h>

class TChatServer
{
private:
    int port;
    int server_socket;
    bool running;
    std::vector<int> client_sockets;
    std::mutex client_mutex;
    std::vector<std::thread> client_threads;
    std::thread accept_thread;

    void accept_clients();
    void handle_clients(int client_sockets);
    void broadcast_msg(const std::string &msg, int sender_socket);
    void remove_client(int client_socket);

public:
    TChatServer(int port);
    ~TChatServer();
    void start();
    void stop();
    void cleanup();
};