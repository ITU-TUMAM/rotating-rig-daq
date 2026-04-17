#include "scanivalve/tcp_transport.hpp"

// --- WINDOWS VE LINUX UYUM BLOĞU BAŞLANGICI ---
#ifdef _WIN32
    #include <winsock2.h>
    #include <ws2tcpip.h>
    #pragma comment(lib, "ws2_32.lib")
    using socket_t = SOCKET;
    #define CLOSE_SOCKET closesocket
    using ssize_t = long long;
#else
    #include <arpa/inet.h>
    #include <netinet/in.h>
    #include <sys/socket.h>
    #include <unistd.h>
    using socket_t = int;
    #define CLOSE_SOCKET ::close
#endif
// --- WINDOWS VE LINUX UYUM BLOĞU BİTİŞİ ---

#include <cstdint>
#include <stdexcept>
#include <utility>
#include <string>

namespace scanivalve {

TcpTransport::TcpTransport(const std::string& host, std::uint16_t port) {
    // Windows için özel başlatma (WSAStartup)
#ifdef _WIN32
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        throw std::runtime_error("TcpTransport: WSAStartup failed");
    }
#endif

    sock_ = ::socket(AF_INET, SOCK_STREAM, 0);
    if (sock_ < 0) {
        throw std::runtime_error("TcpTransport: socket() failed");
    }

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    
    if (::inet_pton(AF_INET, host.c_str(), &addr.sin_addr) <= 0) {
        CLOSE_SOCKET(sock_);
        sock_ = -1;
        throw std::runtime_error("TcpTransport: inet_pton failed");
    }

    if (::connect(sock_, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) < 0) {
        CLOSE_SOCKET(sock_);
        sock_ = -1;
        throw std::runtime_error("TcpTransport: connect() failed");
    }
}

TcpTransport::~TcpTransport() {
    if (sock_ >= 0) {
        CLOSE_SOCKET(sock_);
#ifdef _WIN32
        WSACleanup();
#endif
    }
}

TcpTransport::TcpTransport(TcpTransport&& other) noexcept : sock_(other.sock_) {
    other.sock_ = -1;
}

TcpTransport& TcpTransport::operator=(TcpTransport&& other) noexcept {
    if (this != &other) {
        if (sock_ >= 0) {
            CLOSE_SOCKET(sock_);
        }
        sock_ = other.sock_;
        other.sock_ = -1;
    }
    return *this;
}

void TcpTransport::send(const void* buf, std::size_t n) {
    const auto* p = static_cast<const char*>(buf); // Windows char* bekler
    std::size_t sent = 0;
    while (sent < n) {
        const ssize_t r = ::send(sock_, p + sent, static_cast<int>(n - sent), 0);
        if (r <= 0) {
            throw std::runtime_error("TcpTransport: send() failed");
        }
        sent += static_cast<std::size_t>(r);
    }
}

void TcpTransport::recv(void* buf, std::size_t n) {
    auto* p = static_cast<char*>(buf); // Windows char* bekler
    std::size_t got = 0;
    while (got < n) {
        const ssize_t r = ::recv(sock_, p + got, static_cast<int>(n - got), 0);
        if (r <= 0) {
            throw std::runtime_error("TcpTransport: recv() failed or closed");
        }
        got += static_cast<std::size_t>(r);
    }
}

}  // namespace scanivalve