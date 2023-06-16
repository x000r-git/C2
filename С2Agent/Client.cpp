#pragma once
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <sys/wait.h>
#include "message.h"

/**
 * Server class
 */
class Server {
public:
    Server() : server_port_(2023), backlog_(5), buffer_size_(1024) {}
    Server(int64_t port, int64_t backlog, int64_t buffer_size) : server_port_(port), backlog_(backlog), buffer_size_(buffer_size) {}

    void SetUpServer();
    void Run();
    void Stop();

private:
    
    void HandleClientConnection(int64_t client_socket);

    int64_t buffer_size_ = 0, server_port_ = 0;
    int64_t backlog_ = 0;
    int64_t server_socket_ = 0;
    struct sockaddr_in server_address_;
};