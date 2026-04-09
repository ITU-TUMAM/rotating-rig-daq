# scanivalve-client

A C++17 client library for Scanivalve pressure scanners (DSA/MPS series),
with a clean transport abstraction for testability.

## Features

- POSIX TCP transport for talking to real hardware
- `ITransport` interface for dependency injection
- In-process `FakeTransport` for fast, hermetic unit tests
- Catch2 v3 test suite (fetched automatically via CMake `FetchContent`)
- Python mock device for local development without hardware

## Building

```bash
cmake -B build
cmake --build build --parallel
ctest --test-dir build --output-on-failure
```

## Usage

```cpp
#include <scanivalve/scanivalve_client.hpp>
#include <scanivalve/tcp_transport.hpp>

#include <memory>

auto transport = std::make_unique<scanivalve::TcpTransport>("192.168.1.100", 23);
scanivalve::ScanivalveClient client(std::move(transport));

client.sendCommand("SCAN");
auto frame = client.readFrame();
client.sendCommand("STOP");
```

## Local Development With the Mock

Run the mock device in one terminal:

```bash
python3 tools/mock_scanivalve.py 2323
```

Run the demo client in another:

```bash
./build/scan_demo 127.0.0.1 2323 20
```

## Architecture

The client depends on an `ITransport` interface, which lets the production
`TcpTransport` be swapped for a `FakeTransport` in unit tests. This means
the test suite runs in microseconds with no sockets, no ports, and no CI
flakiness. See `docs/protocol.md` for the wire format.

```
ScanivalveClient ──> ITransport
                      ├── TcpTransport   (production)
                      └── FakeTransport  (tests)
```

## Repository Layout

```
include/scanivalve/   public headers
src/                  library implementation
apps/                 demo executable
tests/                Catch2 unit tests
tools/                mock_scanivalve.py
docs/                 protocol notes
cmake/                CMake helper modules
```

## License

MIT — see [LICENSE](LICENSE).
