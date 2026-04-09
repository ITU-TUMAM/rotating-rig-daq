#include "scanivalve/scanivalve_client.hpp"
#include "scanivalve/tcp_transport.hpp"

#include <cstdint>
#include <cstdlib>
#include <exception>
#include <iostream>
#include <memory>
#include <string>

int main(int argc, char** argv) {
    const std::string host = (argc > 1) ? argv[1] : "127.0.0.1";
    const std::uint16_t port =
        (argc > 2) ? static_cast<std::uint16_t>(std::stoi(argv[2])) : 23;
    const int n_frames = (argc > 3) ? std::stoi(argv[3]) : 20;

    try {
        auto transport = std::make_unique<scanivalve::TcpTransport>(host, port);
        scanivalve::ScanivalveClient client(std::move(transport));
        std::cout << "Connected to " << host << ":" << port << "\n";

        client.sendCommand("SCAN");

        for (int i = 0; i < n_frames; ++i) {
            const auto f = client.readFrame();
            std::cout << "frame " << f.index << " :";
            for (const float p : f.pressures) {
                std::cout << " " << p;
            }
            std::cout << "\n";
        }

        client.sendCommand("STOP");
    } catch (const std::exception& e) {
        std::cerr << "error: " << e.what() << "\n";
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
