#include "server.h"

/**
 * Function starts server on set port
 * @return 0 if OK
 */
int main(){
    int64_t port = 2023, backlog = 5, buffer_size = 1024;
    Server server = Server(port, backlog, buffer_size);
    server.SetUpServer();
    server.Run();
    server.Stop();
    return 0;
}