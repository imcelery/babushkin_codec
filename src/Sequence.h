#pragma once

#include <cstdint>
#include <stdexcept>
#include <vector>
#include "Number32.h"

enum class CompareResult {
    Less,
    WinWin,
    Greater
};

class Sequence {
private:
    std::vector<uint32_t> data_;
    size_t n_;

public:
    explicit Sequence(const std::vector<uint32_t> &data, size_t n)
        : data_(data), n_(n) {
        if (data_.size() != n / 32 + (n % 32 != 0))
            throw std::invalid_argument("Invalid sequence size");

        if (n % 32 != 0 && !data_.empty()) {
            if (data_.back() >= (1ULL << (n % 32)))
                throw std::invalid_argument("Invalid last block");
        }
    }

    [[nodiscard]] CompareResult compare(const Number32 &l, const Number32 &r) const {
        uint64_t rem = l[0];
        uint64_t den = r[0];

        if (den == 0 || rem >= den)
            throw std::invalid_argument("Expected 0 <= l < r");

        size_t remaining = n_;

        for (const uint32_t expected: data_) {
            auto bits = static_cast<unsigned>(   remaining < 32 ? remaining : 32 );

            uint64_t scaled = rem << bits;
            uint64_t lower = den * expected;

            if (scaled < lower)
                return CompareResult::Less;

            rem = scaled - lower;

            if (rem >= den)
                return CompareResult::Greater;

            remaining -= bits;
        }

        return CompareResult::WinWin;
    }
};