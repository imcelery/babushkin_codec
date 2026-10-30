#include <algorithm>

#include <cstdint>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <iterator>
#include <limits>
#include <stdexcept>
#include <string>
#include <vector>

#ifdef _WIN32
#include <fcntl.h>
#include <io.h>
#endif

#include "Codec.h"

using Bits = std::vector<uint32_t>;

Bits pack(const std::vector<uint8_t>& bytes) {
    Bits result;
    result.reserve(bytes.size() / 4 + (bytes.size() % 4 != 0));

    uint32_t block = 0;
    size_t count = 0;

    for (uint8_t byte : bytes) {
        block = (block << 8) | byte;

        if (++count == 4) {
            result.push_back(block);
            block = 0;
            count = 0;
        }
    }

    if (count != 0)
        result.push_back(block);

    return result;
}


std::vector<uint8_t> unpack(const Bits& data, size_t n) {
    if (n % 8 != 0)
        throw std::invalid_argument("Bit count must be divisible by 8");

    std::vector<uint8_t> result;
    result.reserve(n / 8);

    size_t remaining = n;

    for (uint32_t block : data) {
        size_t count = std::min(remaining / 8, size_t{4});

        for (size_t i = count; i > 0; --i) {
            result.push_back(
                static_cast<uint8_t>(block >> (8 * (i - 1)))
            );
        }

        remaining -= count * 8;
    }

    if (remaining != 0)
        throw std::runtime_error("Not enough decoded data");

    return result;
}

std::vector<uint8_t> readFile(const std::string& path) {
    std::ifstream file(path, std::ios::binary);

    if (!file)
        throw std::runtime_error("Cannot open file: " + path);

    std::vector<uint8_t> bytes{
        std::istreambuf_iterator<char>(file),
        std::istreambuf_iterator<char>()
    };

    if (file.bad())
        throw std::runtime_error("Cannot read file: " + path);

    return bytes;
}

void writeFile(const std::string& path,
               const std::vector<uint8_t>& bytes) {
    std::ofstream file(path, std::ios::binary);

    if (!file)
        throw std::runtime_error("Cannot create file: " + path);

    if (!bytes.empty()) {
        file.write(
            reinterpret_cast<const char*>(bytes.data()),
            static_cast<std::streamsize>(bytes.size())
        );
    }

    if (!file)
        throw std::runtime_error("Cannot write file: " + path);
}

uint64_t parseUnsigned(const std::string& s) {
    if (s.empty() || s[0] == '-')
        throw std::invalid_argument("Invalid unsigned integer: " + s);

    size_t pos = 0;
    uint64_t value = std::stoull(s, &pos);

    if (pos != s.size())
        throw std::invalid_argument("Invalid integer: " + s);

    return value;
}

void usage() {
    std::cerr
        << "Usage:\n"
        << "  primecodec encode --string <text>\n"
        << "  primecodec encode --file <input>\n"
        << "  primecodec decode <p> <q> <n> --string\n"
        << "  primecodec decode <p> <q> <n> --file <output>\n";
}

int main(int argc, char* argv[]) {
    try {
        if (argc < 2) {
            usage();
            return 1;
        }

        std::string command = argv[1];

        if (command == "encode") {
            if (argc != 4) {
                usage();
                return 1;
            }

            std::string mode = argv[2];
            std::vector<uint8_t> bytes;

            if (mode == "--string") {
                std::string text = argv[3];
                bytes.assign(text.begin(), text.end());
            }
            else if (mode == "--file") {
                bytes = readFile(argv[3]);
            }
            else {
                usage();
                return 1;
            }

            if (bytes.size() > std::numeric_limits<size_t>::max() / 8)
                throw std::overflow_error("Input too large");

            size_t n = bytes.size() * 8;
            Bits data = pack(bytes);

            auto result = encode(data, n);

            if (!result) {
                std::cerr << "No matching prime pair found\n";
                return 2;
            }

            auto [p, q] = *result;

            std::cout << p << ' ' << q << ' ' << n << '\n';
            return 0;
        }

        if (command == "decode") {
            if (argc != 6 && argc != 7) {
                usage();
                return 1;
            }

            uint64_t p64 = parseUnsigned(argv[2]);
            uint64_t q64 = parseUnsigned(argv[3]);
            uint64_t n64 = parseUnsigned(argv[4]);

            if (p64 > UINT32_MAX || q64 > UINT32_MAX ||
                n64 > std::numeric_limits<size_t>::max())
                throw std::out_of_range("Number out of range");

            uint32_t p = static_cast<uint32_t>(p64);
            uint32_t q = static_cast<uint32_t>(q64);
            size_t n = static_cast<size_t>(n64);

            if (n % 8 != 0)
                throw std::invalid_argument("n must be divisible by 8");

            Bits data = decode(p, q, n);
            auto bytes = unpack(data, n);

            std::string mode = argv[5];

            if ((mode == "--string" && argc != 6) ||
                (mode == "--file" && argc != 7)) {
                usage();
                return 1;
            }

            if (mode == "--file") {
                writeFile(argv[6], bytes);
            }
            else if (mode == "--string") {
#ifdef _WIN32
                _setmode(_fileno(stdout), _O_BINARY);
#endif
                std::cout.write(
                    reinterpret_cast<const char*>(bytes.data()),
                    static_cast<std::streamsize>(bytes.size())
                );
            }
            else {
                usage();
                return 1;
            }

            return 0;
        }

        usage();
        return 1;
    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << '\n';
        return 1;
    }
}
