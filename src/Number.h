#pragma once
#include <cstdint>
#include <cstddef>

class Number {
public:
    Number() = default;
    virtual ~Number() = default;
    virtual uint32_t operator[](size_t i) const = 0;
    [[nodiscard]] virtual size_t size() const = 0;
};
