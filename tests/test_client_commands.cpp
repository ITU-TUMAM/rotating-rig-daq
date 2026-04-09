#include "fake_transport.hpp"
#include "scanivalve/scanivalve_client.hpp"

#include <catch2/catch_test_macros.hpp>

#include <memory>

using scanivalve::ScanivalveClient;
using scanivalve::testing::FakeTransport;

TEST_CASE("sendCommand appends CRLF", "[commands]") {
    auto fake = std::make_unique<FakeTransport>();
    auto* raw = fake.get();
    ScanivalveClient client(std::move(fake));

    client.sendCommand("SCAN");
    REQUIRE(raw->sentAsString() == "SCAN\r\n");
}

TEST_CASE("sendCommand accumulates a sequence of commands", "[commands]") {
    auto fake = std::make_unique<FakeTransport>();
    auto* raw = fake.get();
    ScanivalveClient client(std::move(fake));

    client.sendCommand("SCAN");
    client.sendCommand("STOP");
    client.sendCommand("LIST S");

    REQUIRE(raw->sentAsString() == "SCAN\r\nSTOP\r\nLIST S\r\n");
}

TEST_CASE("sendCommand handles empty string", "[commands]") {
    auto fake = std::make_unique<FakeTransport>();
    auto* raw = fake.get();
    ScanivalveClient client(std::move(fake));

    client.sendCommand("");
    REQUIRE(raw->sentAsString() == "\r\n");
}
