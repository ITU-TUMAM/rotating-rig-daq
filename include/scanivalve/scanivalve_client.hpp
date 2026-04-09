#pragma once

#include "scanivalve/frame.hpp"
#include "scanivalve/transport.hpp"

#include <memory>
#include <string>

namespace scanivalve {

class ScanivalveClient {
public:
    explicit ScanivalveClient(std::unique_ptr<ITransport> transport);

    // Send an ASCII command. CRLF is appended automatically.
    void sendCommand(const std::string& cmd);

    // Read one binary frame: [int32 type][int32 index][int32 nchan][float32 * nchan].
    Frame readFrame();

private:
    std::unique_ptr<ITransport> transport_;
};

}  // namespace scanivalve
