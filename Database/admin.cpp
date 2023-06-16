#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>
#include <iostream>

void getClients(int &socket_fd){
    char buffer[1024] = {0};
    char sql_statement[] = "SELECT IPADDR, NETIPADDR, MAC, LAST_HELLO FROM CLIENTS;";
    send(socket_fd, sql_statement, strlen(sql_statement), 0);

    read(socket_fd, buffer, 1024);
    std::cout << buffer << std::endl;
}

void checkOutput(int &socket_fd){
    char buffer[1024] = {0};
    char sql_statement[] = "";
    send(socket_fd, sql_statement, strlen(sql_statement), 0);

    read(socket_fd, buffer, 1024);
}

bool checkEq(char * a, char * b){
    if (!strcmp(a, b))
        return true;
    return false;
}

int main(int argc, char* argv[]) {
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);

    // Set up socket address
    struct sockaddr_in serv_addr;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(3307);
    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
    char command[1024];
    std::cout << "Доступные команды: clients" << std::endl << "Введите команду -> ";
    std::cin >> command;

    if (checkEq(command, "clients"))
        getClients(sockfd);

    return 0;
}