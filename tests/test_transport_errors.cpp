#include "fake_transport.hpp"
#include "scanivalve/scanivalve_client.hpp"

#include <catch2/catch_test_macros.hpp>

#include <memory>
#include <stdexcept>

using scanivalve::ScanivalveClient;
using scanivalve::testing::FakeTransport;

TEST_CASE("readFrame propagates recv failures", "[transport]") {
    auto fake = std::make_unique<FakeTransport>();
    fake->throw_on_recv = true;
    ScanivalveClient client(std::move(fake));
    REQUIRE_THROWS_AS(client.readFrame(), std::runtime_error);
}

TEST_CASE("sendCommand propagates send failures", "[transport]") {
    auto fake = std::make_unique<FakeTransport>();
    fake->throw_on_send = true;
    ScanivalveClient client(std::move(fake));
    REQUIRE_THROWS_AS(client.sendCommand("SCAN"), std::runtime_error);
}

TEST_CASE("readFrame throws when transport runs out of bytes mid-header",
          "[transport]") {
    auto fake = std::make_unique<FakeTransport>();
    auto* raw = fake.get();
    const char partial[4] = {0, 0, 0, 1};  // only 4 of 12 header bytes
    raw->queueBytes(partial, sizeof(partial));

    ScanivalveClient client(std::move(fake));
    REQUIRE_THROWS_AS(client.readFrame(), std::runtime_error);
}

TEST_CASE("constructor rejects null transport", "[transport]") {
    REQUIRE_THROWS_AS(ScanivalveClient(nullptr), std::invalid_argument);
}
