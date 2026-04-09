#include "fake_transport.hpp"
#include "scanivalve/scanivalve_client.hpp"

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include <cstdint>
#include <memory>
#include <vector>

using Catch::Matchers::WithinAbs;
using scanivalve::Frame;
using scanivalve::ScanivalveClient;
using scanivalve::testing::FakeTransport;

TEST_CASE("readFrame parses a well-formed binary frame", "[frame]") {
    auto fake = std::make_unique<FakeTransport>();
    auto* raw = fake.get();
    const std::vector<float> p = {1.0F, 2.5F, -3.25F, 4.75F};
    raw->queueFrame(/*type=*/1, /*index=*/42, p);

    ScanivalveClient client(std::move(fake));
    const Frame f = client.readFrame();

    REQUIRE(f.type == 1);
    REQUIRE(f.index == 42);
    REQUIRE(f.pressures.size() == 4);
    REQUIRE_THAT(f.pressures[0], WithinAbs(1.0, 1e-6));
    REQUIRE_THAT(f.pressures[1], WithinAbs(2.5, 1e-6));
    REQUIRE_THAT(f.pressures[2], WithinAbs(-3.25, 1e-6));
    REQUIRE_THAT(f.pressures[3], WithinAbs(4.75, 1e-6));
}

TEST_CASE("readFrame can read multiple frames in sequence", "[frame]") {
    auto fake = std::make_unique<FakeTransport>();
    auto* raw = fake.get();
    raw->queueFrame(1, 0, {0.0F, 0.1F});
    raw->queueFrame(1, 1, {1.0F, 1.1F});
    raw->queueFrame(1, 2, {2.0F, 2.1F});

    ScanivalveClient client(std::move(fake));
    for (int i = 0; i < 3; ++i) {
        const Frame f = client.readFrame();
        REQUIRE(f.index == i);
        REQUIRE(f.pressures.size() == 2);
        REQUIRE_THAT(f.pressures[0], WithinAbs(static_cast<double>(i), 1e-6));
        REQUIRE_THAT(f.pressures[1],
                     WithinAbs(static_cast<double>(i) + 0.1, 1e-6));
    }
}

TEST_CASE("readFrame rejects negative channel counts", "[frame]") {
    auto fake = std::make_unique<FakeTransport>();
    auto* raw = fake.get();
    const std::int32_t hdr[3] = {1, 0, -5};
    raw->queueBytes(hdr, sizeof(hdr));

    ScanivalveClient client(std::move(fake));
    REQUIRE_THROWS_AS(client.readFrame(), std::runtime_error);
}

TEST_CASE("readFrame rejects absurdly large channel counts", "[frame]") {
    auto fake = std::make_unique<FakeTransport>();
    auto* raw = fake.get();
    const std::int32_t hdr[3] = {1, 0, 1'000'000};
    raw->queueBytes(hdr, sizeof(hdr));

    ScanivalveClient client(std::move(fake));
    REQUIRE_THROWS_AS(client.readFrame(), std::runtime_error);
}
