#pragma once

#include <cstdint>
#include <vector>

namespace scanivalve {

struct Frame {
    std::int32_t type{};
    std::int32_t index{};
    std::vector<float> pressures;
};

}  // namespace scanivalve
