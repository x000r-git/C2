#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"

std::string ParseMACAddress(const std::vector<uint8_t>& buffer, std::size_t offset) {
    std::string mac;
    static const char* digits = "0123456789ABCDEF";
     if (offset + 6 >= buffer.size()) {
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

bool badCommand(std::string &com){
    if (com.size() != 6)
        return true;
    return false;
}

bool checkEq(char * a, char * b){
    if (!strcmp(a, b))
        return true;
    return false;
}