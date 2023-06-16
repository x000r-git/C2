#include "Client.h"


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    uint32_t port = 2023;
    std::string ipv4 = "192.168.187.128";
    Client client(port, ipv4);
    for (;;) {
        client.SetUpClient();
        client.Run();
        Sleep(5000);
        client.Clean();
    }
    return 0;
}