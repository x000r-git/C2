#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"

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

bool badCommand(std::string &com){
    if (com.size() != 6)
        return true;
    return false;
}

bool checkEq(const char * a, const char * b){
    if (!strcmp(a, b))
        return true;
    return false;
}

TEST_CASE("testing checkEq function") {
    CHECK(checkEq("clientss", "clients") == false);
    CHECK(checkEq("clients", "clients") == true);
    CHECK(checkEq("clies", "clients") == false);
}

TEST_CASE("checking sql statement parsing"){
    std::string first = "SELECT * FROM biba;";
    CHECK(badCommand(first) == true);
    std::string command = "SELECT";
    CHECK(badCommand(command) == false);
}

TEST_CASE("checking mac-address parsing"){
    std::vector<uint8_t> check = {132, 156, 189, 222, 109, 155};
    CHECK(ParseMACAddress(check, 0) == "84-9C-BD-DE-6D-9B");
}