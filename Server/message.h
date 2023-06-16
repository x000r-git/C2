#include <vector>
#include <string>

#include "../config.h"

/**
 * Class for message from Server
 */
class MessageServer {
public:
    int64_t Serialization(std::vector<uint8_t>& buffer);
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

    int64_t Deserialization(const std::vector<uint8_t>& buffer);
    void Print();
private:
    std::string local_ipv4_ = "";
    std::string mac_address_ = "";
    CommandsClient command;
    uint32_t data_size_ = 0;
    std::vector<uint8_t> data;
};