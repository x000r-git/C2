#pragma once

#define WIN32_LEAN_AND_MEAN
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <vector>
#include <string>
#include <map>
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <Iphlpapi.h>
#include <sstream>
#include <Assert.h>
#pragma comment(lib, "iphlpapi.lib")
#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")

#include "../config.h"

/**
 * Class for Message from Server
 */
class MessageServer {
public:

    MessageServer() = default;

    int64_t Deserialization(const std::vector<uint8_t>& buffer);

    ~MessageServer() = default;

private:
    CommandsServer command;
    uint32_t data_size_ = 0;
    std::vector<uint8_t> data;
};

/**
 * Class for message from Client
 */
class MessageClient {
public:

    MessageClient() = default;

    int64_t Serialization(std::vector<uint8_t>& buffer);

    void SetLocalIP(std::string ip) {
        local_ip_ = ip;
    }

    void SetMACAddress(std::string mac) {
        mac_address_ = mac; 
    }

    void SetCommand(CommandsClient command) {
        command_ = command;
    }

    void SetDataSize(uint32_t data_size) {
        data_size_ = data_size;
    }

    ~MessageClient() = default;

private:
    std::string local_ip_ = "";
    std::string mac_address_ = "";
    CommandsClient command_;
    uint32_t data_size_ = 0;
    std::vector<uint8_t> data;
};