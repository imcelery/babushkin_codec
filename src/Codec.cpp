#include "Codec.h"

#include <algorithm>
#include <stdexcept>

#include "Generator32.h"
#include "Sequence.h"
#include "Number32.h"

std::optional<std::pair<uint32_t, uint32_t> > encode(const std::vector<uint32_t> &data, size_t n) {
    Sequence sequence(data, n);
    Generator32 generator;

    while (generator.left() && generator.right()) {
        auto &l = static_cast<const Number32 &>(*generator.left());
        auto &r = static_cast<const Number32 &>(*generator.right());

        if (l[0] >= r[0]) {
            if (!generator.nextRight())
                break;
            continue;
        }

        switch (sequence.compare(l, r)) {
            case CompareResult::WinWin:
                return std::pair{l[0], r[0]};

            case CompareResult::Less:
                if (!generator.nextLeft())
                    return std::nullopt;
                break;

            case CompareResult::Greater:
                if (!generator.nextRight())
                    return std::nullopt;
                break;
        }
    }

    return std::nullopt;
}

std::vector<uint32_t> decode(uint32_t p, uint32_t q, size_t n) {
    if (q == 0 || p >= q)
        throw std::invalid_argument("Expected 0 <= p < q");

    std::vector<uint32_t> result;
    result.reserve(n / 32 + (n % 32 != 0));

    uint64_t rem = p;

    while (n > 0) {
        unsigned bits = static_cast<unsigned>(std::min(n, size_t{32}));

        uint64_t scaled = rem << bits;

        uint32_t block = static_cast<uint32_t>(scaled / q);
        rem = scaled % q;

        result.push_back(block);
        n -= bits;
    }

    return result;
}
