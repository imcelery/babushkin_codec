#pragma once

#include <cstdint>
#include <cstddef>
#include <optional>
#include <utility>
#include <vector>

std::optional<std::pair<uint32_t, uint32_t> > encode(const std::vector<uint32_t> &data, size_t n);

std::vector<uint32_t> decode(uint32_t p, uint32_t q, size_t n);
