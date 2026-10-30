#include "Codec.h"

#include <cstdint>
#include <iostream>
#include <stdexcept>
#include <vector>

void require(bool value, const char* message) {
    if (!value)
        throw std::runtime_error(message);
}

int main() {
    // Fraction 2/3 = 0.101010... (binary); first byte is 0xAA.
    require(decode(2, 3, 8) == std::vector<uint32_t>{0xAA},
            "decode(2,3,8) must yield AA");

    // Input is one byte 'A' = 0x41, stored as a right-aligned block.
    const std::vector<uint32_t> data = {0x41};
    auto pair = encode(data, 8);
    require(pair.has_value(), "encode(A) must find a pair");
    require(decode(pair->first, pair->second, 8) == data,
            "A must survive encode/decode");

    std::cout << "Smoke tests passed\n";
}
