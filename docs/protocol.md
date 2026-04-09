# Scanivalve Wire Protocol (as implemented here)

This document describes the simplified protocol that this client and the
included mock server speak. It is **not** a faithful reproduction of any
specific Scanivalve product manual — it is a clean subset suitable for
development and testing. When targeting a real DSA3217, DSA3218, or
MPS4264, consult the device's user manual and adjust both the client's
`readFrame()` and the mock's `make_frame()` to match.

## Transport

- TCP/IP, single connection per client.
- Default port: `23` (telnet) on real hardware, `2323` for the mock.
- Little-endian byte order on the wire.

## Commands (client → device)

ASCII text, terminated by `\r\n`.

| Command  | Meaning                                       |
|----------|-----------------------------------------------|
| `SCAN`   | Begin streaming binary frames at FPS rate     |
| `STOP`   | Halt streaming                                |
| `LIST S` | Return scanner configuration (ASCII)          |
| `STATUS` | Return current state (`READY` or `SCAN`)      |

## Binary Frame (device → client during SCAN)

Each frame is a contiguous block:

```
offset  size  field
0       4     int32   packet type   (1 = data frame)
4       4     int32   frame index   (monotonically increasing)
8       4     int32   nchan         (number of channels)
12      4*N   float32 pressures[N]  (engineering units)
```

Total size: `12 + 4 * nchan` bytes.

## Notes for Real Hardware

Real Scanivalve packets typically include additional fields between the
header and the pressure array — temperature readings, timestamps, status
flags. The exact layout varies by model and firmware. The `Frame` struct
and `readFrame()` parser in this library are intentionally minimal so
they can be extended without disturbing the transport abstraction.
