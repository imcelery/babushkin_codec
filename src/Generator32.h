#pragma once
#include <bitset>
#include <cstddef>

#include "PrimeGenerator.h"
#include <cstdint>

#include "Number32.h"


class Generator32 final : public PrimeGenerator {
    static constexpr size_t LIMIT = 100000;
    std::bitset<LIMIT> data_;
    size_t checked_ = 1;
    size_t left_idx_ = 0, right_idx_ = 0;
    Number32 left_num_ = Number32(2), right_num_ = Number32(2);

    void expand() {
        uint64_t p = 2 * checked_ + 1;

        for (uint64_t i = p * p / 2; i < LIMIT; i += p) {
            data_.set(static_cast<size_t>(i));
        }

        do {
            ++checked_;
        } while (checked_ < LIMIT && data_.test(checked_));
    }

    void ensure_sieved(uint64_t n) {
        while (checked_ < LIMIT) {
            uint64_t p = 2 * checked_ + 1;

            if (p * p > n)
                break;

            expand();
        }
    }

    bool advance(size_t &idx, Number32 &number) {
        if (idx == LIMIT)
            return false;

        for (size_t i = idx + 1; i < LIMIT; ++i) {
            uint64_t candidate = 2ULL * i + 1;

            ensure_sieved(candidate);

            if (!data_.test(i)) {
                idx = i;
                number = Number32( static_cast<uint32_t>(candidate) );
                return true;
            }
        }

        idx = LIMIT;
        return false;
    }

public:
    Generator32() {
        data_.set(0); // 1 is not prime
        nextRight(); // initial state: left = 2, right = 3
    }

    ~Generator32() override = default;

    [[nodiscard]] const Number *left() const override {
        return left_idx_ == LIMIT ? nullptr : &left_num_;
    }

    [[nodiscard]] const Number *right() const override {
        return right_idx_ == LIMIT ? nullptr : &right_num_;
    }

    bool nextLeft() override {
        return advance(left_idx_, left_num_);
    }

    bool nextRight() override {
        return advance(right_idx_, right_num_);
    }
};
