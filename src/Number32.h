#pragma once
#include "Number.h"
#include <cstdint>

class Number32 final : public Number {
    uint32_t value_;

public:
    explicit Number32(uint32_t value)
        : value_(value) {
    }

    uint32_t operator[](size_t i) const override {
        return i == 0 ? value_ : 0;
    }

    [[nodiscard]] size_t size() const override {
        return 1;
    }
};
