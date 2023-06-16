#include "message.h"
#include <iomanip>
#include <arpa/inet.h>

/**
 * Function parses MAC-address from protocol to string
 * @param buffer Data
 * @param offset Offset to MAC-address place
 * @return MAC-address string
 */
std::string ParseMACAddress(const std::vector<uint8_t>& buffer, std::size_t offset) {
    std::string mac;
    static const char* digits = "0123456789ABCDEF";
     if (offset + 6 > buffer.size()) {
        throw std::string("MACAddress: lack buffer\n");
    }

    for (int64_t i = 0; i < 6; ++i) {
        mac += digits[(buffer[offset + i] >> 4) & 0xf];
        mac += digits[buffer[offset + i] & 0xf];
        if (i < 5) {
            mac += "-";
        }
    }
    
    return mac;
}

/**
 * Function parses input message and creates MessageClient structure
 * @param buffer
 * @return 0 if OK
 */
int64_t MessageClient::Deserialization(const std::vector<uint8_t>& buffer) {
    try {
        uint32_t magic_number = *reinterpret_cast<const uint32_t*>(&buffer[0]);

        if (magic_number != 0x43324332) {
            throw std::string("Wrong magic number\n");
        }
        struct in_addr ip_addr;
        ip_addr.s_addr = *reinterpret_cast<const uint32_t*>(&buffer[4]);
        this->local_ipv4_ = inet_ntoa(ip_addr);
        this->mac_address_ = ParseMACAddress(buffer, 8);
        this->command = (CommandsClient)*reinterpret_cast<const uint32_t *>(&buffer[14]);
        this->data_size_ = *reinterpret_cast<const uint32_t *>(&buffer[18]);
        this->data.insert(this->data.end(), buffer.begin() + 22, buffer.end());
    } catch(std::string error_message) {
        std::cerr << error_message;
        return 1;
    }
    return 0;
}

/**
 * Function serializing message to client
 * @param buffer
 * @return 0 if OK
 */
int64_t MessageServer::Serialization(std::vector<uint8_t>& buffer) {
    buffer.resize(4 + sizeof(this->command) + sizeof(this->data_size_));
    *reinterpret_cast<uint32_t *>(&buffer[0]) = 0x42324232;
    *reinterpret_cast<uint32_t *>(&buffer[4]) = (uint32_t)this->command;
    *reinterpret_cast<uint32_t *>(&buffer[8]) = this->data_size_;
    buffer.insert(buffer.end(), this->data.begin(), this->data.end());
    return 0;
}

/**
 * Function prints command
 */
void MessageClient::Print() {
    std::cout << this->local_ipv4_ << "\n";
    std::cout << this->mac_address_ << "\n";
    switch (this->command) {
        case CommandsClient::HELLO:
            std::cout << "CommandsClient::HELLO\n";
            break;
        default:
        break;
    }
}