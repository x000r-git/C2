#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <vector>
#include <sqlite3.h>
#include <unistd.h>
#include <cstring>

#define BUFFER_SIZE 1024//INSERT INTO CLIENTS(IPADDR, NETIPADDR, MAC, LAST_HELLO) VALUES ('1.1.1.1', '2.2.2.2', '11:22:33:44:55:66', 13)
#define SERVER_PORT 3307

void create_tables(sqlite3 *db_p){
    // Three tables
    std::vector<std::string> sql;
    char *err = 0;
    // clients table for agents
    sqlite3_exec(db_p, "CREATE TABLE CLIENTS(ID INT PRIMARY KEY, IPADDR CHAR(15), NETIPADDR CHAR(15), MAC CHAR(17), LAST_HELLO INT)", 0, 0, &err);
    sqlite3_exec(db_p, "CREATE TABLE COMMANDS(ID INT PRIMARY KEY, COMMAND TEXT, CLIENT_ID INT)", 0, 0, &err);
    sqlite3_exec(db_p, "CREATE TABLE OUTPUT(ID INT PRIMARY KEY, IPADDR CHAR(15), NETIPADDR CHAR(15), MAC CHAR(17), OUT TEXT)", 0, 0, &err);
}

bool badCommand(std::string &com){
    if (com.size() != 6)
        return true;
    return false;
}

int main(){
    int serverSocket, clientSocket;
    struct sockaddr_in serverAddress, clientAddress;
    socklen_t clientAddressLength;

    sqlite3 *database_ptr;
    int database_descr = sqlite3_open(NULL, &database_ptr);
    sqlite3_stmt * stmt;

    if (database_descr){
        std::cout << "Проблемы с базой" << std::endl;
    }

    create_tables(database_ptr);

    // Creating serverSocket
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket < 0){
        std::cerr << "Серверный сокет не создан. Ошибка." << std::endl;
        return 1;
    }

    // Binding serverSocket
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = INADDR_ANY;
    serverAddress.sin_port = htons(SERVER_PORT);
    if (bind(serverSocket, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) < 0){
        std::cerr << "Скорее всего порт " << SERVER_PORT << " недоступен. Ошибка." << std::endl;
        return 2;
    }

    // Listening for connections
    if (listen(serverSocket, 5) < 0){
        std::cerr << "Прослушивание не запущено. Ошибка." << std::endl;
        return 3;
    }

    std::cout << "База данных запущена" << std::endl;

    while ((clientSocket = accept(serverSocket, (struct sockaddr *)&clientAddress, &clientAddressLength))){
        ssize_t bytesRead;
        char *errMsg = 0;
        std::vector<char> buffer;
        char recv_buffer[BUFFER_SIZE];

        bytesRead = read(clientSocket, recv_buffer, BUFFER_SIZE);
        buffer.insert(buffer.end(), recv_buffer, recv_buffer + bytesRead);

        std::string command(buffer.begin(), buffer.begin()+6);
        
        if (badCommand(command)){
            close(clientSocket);
            continue;
        }

        const char *state = buffer.data();

        if (command == "SELECT") {
            std::vector<char> output;
            int operation_descr = sqlite3_prepare_v2(database_ptr, state, -1, &stmt, NULL);
            if (operation_descr != SQLITE_OK) {
                std::cout << "Ошибка запроса" << std::endl;
            } else {
                while ((operation_descr = sqlite3_step(stmt) == SQLITE_ROW)) {
                    int countColumns = sqlite3_column_count(stmt);
                    for (int i = 0; i < countColumns; i++) {
                        char * output_buffer = (char *) sqlite3_column_text(stmt, i);
                        for (int i = 0; i < strlen(output_buffer); i++)
                            output.push_back(output_buffer[i]);
                        output.push_back(' ');
                    }
                    output.push_back('\n');
                }
            }
            for (char i: output){
                std::cout << i;
            }
            send(clientSocket, output.data(), output.size(), 0);
        } else if ((command == "INSERT") || (command == "UPDATE")) {
            std::cout << "INSERT OR UPDATE" << std::endl;
            sqlite3_exec(database_ptr, state, nullptr, nullptr, &errMsg);
            std::cout << "записано" << std::endl;
        }
        close(clientSocket);
    }

    std::cout << "Server stopped" <<std::endl;
    close(serverSocket);
}
