#include "server.h"

// MSYS COMMANDS: export PATH=$PATH:/mingw64/bin/; g++ client.cpp -lwsock32


void Server::SetUpServer() {
    server_socket_ = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket_ < 0){
        std::cerr << "Серверный сокет не создан. Ошибка.\n";
        return;
    }

    // Binding server
    server_address_.sin_family = AF_INET;
    server_address_.sin_addr.s_addr = INADDR_ANY;
    server_address_.sin_port = htons(server_port_);

    if (bind(server_socket_, (struct sockaddr *)&server_address_, sizeof(server_address_)) < 0){
        std::cerr << "Скорее всего порт " << server_port_ << " недоступен. Ошибка.\n";
        return ;
    }

    // Set listen mod
    if (listen(server_socket_, 5) < 0){
        std::cerr << "Прослушивание не запущено. Ошибка.\n";
        return;
    }

    std::cout << "Server created\n";
}

void Server::Run() {
    int64_t client_socket;
    socklen_t client_address_length;
    struct sockaddr_in client_address;

    while ((client_socket = accept(server_socket_, (struct sockaddr *)&client_address, &client_address_length))) {

        pid_t pid = fork();
        if (pid == 0){
            HandleClientConnection(client_socket);
            exit(0);
        }
    }
}

void Server::Stop() {
    std::cout << "Server stopped\n";
    close(server_socket_);
}

void Server::HandleClientConnection(int64_t clientSocket){
    std::cout << "New connection\n";
    ssize_t bytes_read;
    std::vector<uint8_t> buffer;
    char* recv_buffer = new char[this->buffer_size_];

    if ((bytes_read = read(clientSocket, recv_buffer, buffer_size_)) > 0){
        buffer.insert(buffer.end(), recv_buffer, recv_buffer + bytes_read);
    }

    delete[] recv_buffer;

    MessageClient* client_message = new MessageClient();
    if (client_message->Deserialization(buffer)) {
        std::cerr << "Deserialization failed\n";
        delete client_message;
        return;
    }
    
    client_message->Print();
    delete client_message;
    close(clientSocket);
}