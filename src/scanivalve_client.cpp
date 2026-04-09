#include "scanivalve/scanivalve_client.hpp"

#include <cstdint>
#include <stdexcept>
#include <utility>

namespace scanivalve {

namespace {
constexpr std::int32_t kMaxChannels = 4096;
}

ScanivalveClient::ScanivalveClient(std::unique_ptr<ITransport> transport)
    : transport_(std::move(transport)) {
    if (!transport_) {
        throw std::invalid_argument("ScanivalveClient: null transport");
    }
}

void ScanivalveClient::sendCommand(const std::string& cmd) {
    const std::string line = cmd + "\r\n";
    transport_->send(line.data(), line.size());
}

Frame ScanivalveClient::readFrame() {
    std::int32_t hdr[3];
    transport_->recv(hdr, sizeof(hdr));

    Frame f;
    f.type = hdr[0];
    f.index = hdr[1];
    const std::int32_t nchan = hdr[2];

    if (nchan <= 0 || nchan > kMaxChannels) {
        throw std::runtime_error("ScanivalveClient: bad channel count");
    }

    f.pressures.resize(static_cast<std::size_t>(nchan));
    transport_->recv(f.pressures.data(),
                     static_cast<std::size_t>(nchan) * sizeof(float));
    return f;
}

}  // namespace scanivalve
