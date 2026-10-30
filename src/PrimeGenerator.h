#pragma once
#include "Number.h"

class PrimeGenerator {
public:
    virtual ~PrimeGenerator() = default;

    [[nodiscard]] virtual const Number* left() const = 0;

    [[nodiscard]] virtual const Number* right() const = 0;

    virtual bool nextLeft() = 0;

    virtual bool nextRight() = 0;
};
