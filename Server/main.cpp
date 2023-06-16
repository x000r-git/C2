#include "server.h"


int main(){
    int64_t port = 2023, backlog = 5, buffer_size = 1024;
    Server server = Server(port, backlog, buffer_size);
    server.SetUpServer();
    server.Run();
    server.Stop();
}