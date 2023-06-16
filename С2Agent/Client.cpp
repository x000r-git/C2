#include "Client.h"

void Client::SetUpClient() {
   
    // Initialize Winsock
    if (WSAStartup(MAKEWORD(2, 2), &wsa_data_) != 0) {
        std::cerr << "Failed to initialize winsock\n";
        return;
    }

    // Create a socket
    if ((client_socket_ = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) {
        std::cerr << "Failed to create socket\n";
        WSACleanup();
        return;
    }

    // Set up server details
    server_addr_.sin_family = AF_INET;
    server_addr_.sin_port = htons(port_);
    server_addr_.sin_addr.s_addr = inet_addr(ipv4_.c_str());
    /*
    if (!InetPton(AF_INET, (PCWSTR)"192.168.187.128", &serverAddr.sin_addr.s_addr)) {
        std::cerr << "Failed in InetPton" << std::endl;
        WSACleanup();
        return 1;
    }
    */
    return;
}
#define WORKING_BUFFER_SIZE 15000
#define MAX_TRIES 3

#define MALLOC(x) HeapAlloc(GetProcessHeap(), 0, (x))
#define FREE(x) HeapFree(GetProcessHeap(), 0, (x))
int64_t Client::GetMetaData(MessageClient* client_message) {
    struct MetaData {
        std::string ipv4 = "";
        std::string mac = "";
        std::wstring description = L"";
    };
    std::map<int64_t, MetaData> map;

    DWORD dwSize = 0;
    DWORD dwRetVal = 0;

    unsigned int i = 0;

    // Set the flags to pass to GetAdaptersAddresses
    ULONG flags = GAA_FLAG_INCLUDE_PREFIX;

    // default to unspecified address family (both)
    ULONG family = AF_UNSPEC;

    LPVOID lpMsgBuf = NULL;

    PIP_ADAPTER_ADDRESSES pAddresses = NULL;
    ULONG outBufLen = 0;
    ULONG Iterations = 0;

    PIP_ADAPTER_ADDRESSES pCurrAddresses = NULL;
    PIP_ADAPTER_UNICAST_ADDRESS pUnicast = NULL;
    PIP_ADAPTER_ANYCAST_ADDRESS pAnycast = NULL;
    PIP_ADAPTER_MULTICAST_ADDRESS pMulticast = NULL;
    IP_ADAPTER_DNS_SERVER_ADDRESS* pDnServer = NULL;
    IP_ADAPTER_PREFIX* pPrefix = NULL;


    family = AF_INET;
    

    outBufLen = WORKING_BUFFER_SIZE;

    do {

        pAddresses = (IP_ADAPTER_ADDRESSES*)MALLOC(outBufLen);
        if (pAddresses == NULL) {
            exit(1);
        }

        dwRetVal =
            GetAdaptersAddresses(family, flags, NULL, pAddresses, &outBufLen);

        if (dwRetVal == ERROR_BUFFER_OVERFLOW) {
            FREE(pAddresses);
            pAddresses = NULL;
        }
        else {
            break;
        }

        Iterations++;

    } while ((dwRetVal == ERROR_BUFFER_OVERFLOW) && (Iterations < MAX_TRIES));

    if (dwRetVal == NO_ERROR) {
        pCurrAddresses = pAddresses;
        while (pCurrAddresses) {
            if (pCurrAddresses->PhysicalAddressLength != 0) {
                char* byte = new char[3];
                map[pCurrAddresses->IfIndex].description = std::wstring(pCurrAddresses->Description);
                for (i = 0; i < (int)pCurrAddresses->PhysicalAddressLength; i++) {
                    if (i == (pCurrAddresses->PhysicalAddressLength - 1)) {
                        snprintf(byte, 3, "%.2X", (int)pCurrAddresses->PhysicalAddress[i]);
                        map[pCurrAddresses->IfIndex].mac += std::string(byte);

                    }
                    else {
                        snprintf(byte, 3, "%.2X", (int)pCurrAddresses->PhysicalAddress[i]);
                        map[pCurrAddresses->IfIndex].mac += std::string(byte) + "-";
                    }
                }
                delete byte;
            }
           
            pCurrAddresses = pCurrAddresses->Next;
        }
    }

    if (pAddresses) {
        FREE(pAddresses);
    }

    i = 0;

    PMIB_IPADDRTABLE pIPAddrTable;
    dwSize = 0;
    dwRetVal = 0;
    IN_ADDR IPAddr;

    lpMsgBuf;

    pIPAddrTable = (MIB_IPADDRTABLE*)MALLOC(sizeof(MIB_IPADDRTABLE));

    if (pIPAddrTable) {
        if (GetIpAddrTable(pIPAddrTable, &dwSize, 0) == ERROR_INSUFFICIENT_BUFFER) {
            FREE(pIPAddrTable);
            pIPAddrTable = (MIB_IPADDRTABLE*)MALLOC(dwSize);

        }
        if (pIPAddrTable == NULL) {
            exit(1);
        }
    }
    if ((dwRetVal = GetIpAddrTable(pIPAddrTable, &dwSize, 0)) != NO_ERROR) {
        if (FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, dwRetVal, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),       // Default language
            (LPTSTR)&lpMsgBuf, 0, NULL)) {
            LocalFree(lpMsgBuf);
        }
    }

    for (i = 0; i < (int)pIPAddrTable->dwNumEntries; i++) {
        printf("\n\tInterface Index[%d]:\t%ld\n", i, pIPAddrTable->table[i].dwIndex);
        IPAddr.S_un.S_addr = (u_long)pIPAddrTable->table[i].dwAddr;
        if (map.contains(pIPAddrTable->table[i].dwIndex)) {
            map[pIPAddrTable->table[i].dwIndex].ipv4 = std::string(inet_ntoa(IPAddr));
        }
    }

    if (pIPAddrTable) {
        FREE(pIPAddrTable);
        pIPAddrTable = NULL;
    }

    for (auto& elem : map) {
        client_message->SetLocalIP(elem.second.ipv4);
        client_message->SetMACAddress(elem.second.mac);
        break;
    }

    return 0;
}

int64_t Client::ExecuteCommand(MessageClient* client_message) {
    switch (this->curr_command) {
    case (CommandsServer::NOTHING):
        client_message->SetCommand(CommandsClient::HELLO);
        client_message->SetDataSize(0);
        break;
    default:
        break;
    }

    return 0;
}

void Client::Run() {
    MessageClient* client_message = new MessageClient();

    if (GetMetaData(client_message)) {
        std::cerr << "Failed to get metadata\n";
        Clean();
        delete client_message;
        return;
    }

    if (ExecuteCommand(client_message)) {
        std::cerr << "Failed to execute command\n";
        Clean();
        delete client_message;
        return;
    }

    if (Connect(client_message)) {
        std::cerr << "Failed to connect\n";
        Clean();
        delete client_message;
        return;
    }

    delete client_message;
}

int64_t Client::SendMessageClient(MessageClient* client_message) {
    std::vector<uint8_t> buffer;
    if (client_message->Serialization(buffer)) {
        std::cerr << "Serialization failed\n";
        return 1;
    }

    char* message = new char[buffer.size() + 1];
    std::copy(buffer.begin(), buffer.end(), message);
    message[buffer.size()] = 0;

    if (send(client_socket_, message, buffer.size() + 1, 0) < 0) {
        std::cerr << "Failed to send data to server\n";
        delete[] message;
        return 1;
    }

    delete[] message;
    return 0;
}

int64_t Client::RecvMessageServer() {
    std::size_t bytes_read;
    std::vector<uint8_t> buffer;
    char* recv_buffer = new char[buffer_size_];
    return 0;
    while ((bytes_read = recv(client_socket_, recv_buffer, buffer_size_, 0)) > 0) {
        buffer.insert(buffer.end(), recv_buffer, recv_buffer + bytes_read);
    }

    delete[] recv_buffer;

    MessageServer* server_message = new MessageServer();
    if (server_message->Deserialization(buffer)) {
        std::cerr << "Failed to deserialization message...\n";
        return 1;
    }

    //this->curr_command = server_message

    return 0;
}

int64_t Client::Connect(MessageClient* client_message) {
    int64_t temp_counter = 5;
    bool connectFlag = false;

    while (temp_counter > 0) {
        if (connect(client_socket_, reinterpret_cast<struct sockaddr*>(&server_addr_), sizeof(server_addr_)) < 0) {
            temp_counter--;
        }
        else {
            connectFlag = true;
            break;
        }
    }

    if (temp_counter == 0) {
        std::cerr << "Failed to connect to server...\n";
        return 1;
    }

    if (SendMessageClient(client_message)) {
        std::cerr << "Failed to send messsage...\n";
        return 1;
    }

    if (RecvMessageServer()) {
        std::cerr << "Failed to recv messsage...\n";
        return 1;
    }

    return 0;   
}

void Client::Clean() {
    closesocket(client_socket_);
    WSACleanup();
}