#pragma once

#include <cstddef>

namespace scanivalve {

// Abstract byte-stream transport. Implementations must read/write exactly
// the requested number of bytes or throw on failure.
struct ITransport {
    virtual ~ITransport() = default;
    virtual void send(const void* buf, std::size_t n) = 0;
    virtual void recv(void* buf, std::size_t n) = 0;
};

}  // namespace scanivalve
