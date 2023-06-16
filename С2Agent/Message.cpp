#pragma once

#include "Message.h"

/**
 * Function that serializes MAC to uint
 * @param s MAC-address string
 * @param bytes In which bytes we place serialized MAC
 * @return 0 if OK
 */
uint64_t FromMACAddressToUint64(std::string const& s, char* bytes) {
    unsigned char a[6];
    int last = -1;
    int rc = sscanf_s(s.c_str(), "%hhx-%hhx-%hhx-%hhx-%hhx-%hhx%n",
        a + 0, a + 1, a + 2, a + 3, a + 4, a + 5,
        &last);
    if (rc != 6 || s.size() != last)
        throw std::runtime_error("invalid mac address format " + s);
    memcpy(bytes, a, 6);

    return 0;
}

/**
 * Function for serialization
 * @param buffer  buffer to use in serialize
 * @return 0 if OK
 */
int64_t MessageClient::Serialization(std::vector<uint8_t>& buffer) {
    buffer.resize(4 + 4 + 6 + sizeof(this->command_) + sizeof(this->data_size_));
    *reinterpret_cast<uint32_t*>(&buffer[0]) = 0x43324332;
    *reinterpret_cast<uint32_t*>(&buffer[4]) = inet_addr(this->local_ip_.c_str());
    char* bytes = new char[7];
    uint64_t mac = FromMACAddressToUint64(this->mac_address_, bytes);
    memcpy(reinterpret_cast<uint8_t*>(&buffer[8]), bytes, 6);
    delete[] bytes;
    *reinterpret_cast<uint32_t*>(&buffer[14]) = (uint32_t)this->command_;
    *reinterpret_cast<uint32_t*>(&buffer[18]) = this->data_size_;
    buffer.insert(buffer.end(), this->data.begin(), this->data.end());
    return 0;
}

/**
 * Function for deserialization
 * @param buffer  buffer to use in deserialize
 * @return 0 if OK
 */
int64_t MessageServer::Deserialization(const std::vector<uint8_t>& buffer) {
    try {
        uint32_t magic_number = *reinterpret_cast<const uint32_t*>(&buffer[0]);

        if (magic_number != 0x43324332) {
            throw std::string("Wrong magic number\n");
        }

        this->command = (CommandsServer) *reinterpret_cast<const uint32_t*>(&buffer[4]);
        this->data_size_ = *reinterpret_cast<const uint32_t*>(&buffer[8]);
        this->data.insert(this->data.end(), buffer.begin() + 12, buffer.end());
    }
    catch (std::string error_message) {
        std::cerr << error_message;
        return 1;
    }
    return 0;
}