#pragma once

#include "scanivalve/transport.hpp"

#include <cstdint>
#include <cstring>
#include <deque>
#include <stdexcept>
#include <string>
#include <vector>

namespace scanivalve::testing {

// In-process fake: scripted recv bytes, captured send bytes.
class FakeTransport : public ITransport {
public:
    std::deque<std::uint8_t> rx;       // bytes the client will read
    std::vector<std::uint8_t> tx;      // bytes the client has written
    bool throw_on_recv{false};
    bool throw_on_send{false};

    void queueBytes(const void* p, std::size_t n) {
        const auto* b = static_cast<const std::uint8_t*>(p);
        rx.insert(rx.end(), b, b + n);
    }

    void queueFrame(std::int32_t type,
                    std::int32_t index,
                    const std::vector<float>& pressures) {
        const std::int32_t nchan = static_cast<std::int32_t>(pressures.size());
        queueBytes(&type, sizeof(type));
        queueBytes(&index, sizeof(index));
        queueBytes(&nchan, sizeof(nchan));
        queueBytes(pressures.data(), pressures.size() * sizeof(float));
    }

    std::string sentAsString() const {
        return std::string(tx.begin(), tx.end());
    }

    void send(const void* buf, std::size_t n) override {
        if (throw_on_send) {
            throw std::runtime_error("FakeTransport: simulated send failure");
        }
        const auto* b = static_cast<const std::uint8_t*>(buf);
        tx.insert(tx.end(), b, b + n);
    }

    void recv(void* buf, std::size_t n) override {
        if (throw_on_recv) {
            throw std::runtime_error("FakeTransport: simulated recv failure");
        }
        if (rx.size() < n) {
            throw std::runtime_error("FakeTransport: not enough bytes queued");
        }
        auto* out = static_cast<std::uint8_t*>(buf);
        for (std::size_t i = 0; i < n; ++i) {
            out[i] = rx.front();
            rx.pop_front();
        }
    }
};

}  // namespace scanivalve::testing
