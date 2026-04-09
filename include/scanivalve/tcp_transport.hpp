#pragma once

#include "scanivalve/transport.hpp"

#include <cstdint>
#include <string>

namespace scanivalve {

class TcpTransport : public ITransport {
public:
    TcpTransport(const std::string& host, std::uint16_t port);
    ~TcpTransport() override;

    TcpTransport(const TcpTransport&) = delete;
    TcpTransport& operator=(const TcpTransport&) = delete;
    TcpTransport(TcpTransport&&) noexcept;
    TcpTransport& operator=(TcpTransport&&) noexcept;

    void send(const void* buf, std::size_t n) override;
    void recv(void* buf, std::size_t n) override;

private:
    int sock_{-1};
};

}  // namespace scanivalve
