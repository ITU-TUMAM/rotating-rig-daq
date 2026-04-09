#!/usr/bin/env python3
"""Mock Scanivalve TCP server for local testing without hardware.

Run with: python3 mock_scanivalve.py [port]
Default port is 2323 (use 23 if running as root).
"""
import math
import random
import socket
import struct
import sys
import threading
import time

HOST = "127.0.0.1"
DEFAULT_PORT = 2323
NCHAN = 16
FPS = 100  # frames per second when scanning


def make_frame(frame_idx: int) -> bytes:
    """Build a binary packet:
    [int32 type=1][int32 frame][int32 nchan][float32 * nchan]
    """
    pressures = [
        10.0 * math.sin(2 * math.pi * (frame_idx / FPS) + i * 0.1)
        + random.gauss(0, 0.05)
        for i in range(NCHAN)
    ]
    header = struct.pack("<iii", 1, frame_idx, NCHAN)
    body = struct.pack(f"<{NCHAN}f", *pressures)
    return header + body


def handle_client(conn: socket.socket, addr) -> None:
    print(f"[mock] client connected: {addr}")
    scanning = threading.Event()
    frame_idx = [0]

    def stream() -> None:
        while scanning.is_set():
            try:
                conn.sendall(make_frame(frame_idx[0]))
                frame_idx[0] += 1
                time.sleep(1.0 / FPS)
            except OSError:
                break

    buf = b""
    try:
        while True:
            data = conn.recv(1024)
            if not data:
                break
            buf += data
            while b"\r" in buf or b"\n" in buf:
                line, _, buf = buf.replace(b"\r", b"\n").partition(b"\n")
                cmd = line.decode(errors="ignore").strip().upper()
                if not cmd:
                    continue
                print(f"[mock] cmd: {cmd}")
                if cmd == "SCAN":
                    if not scanning.is_set():
                        scanning.set()
                        threading.Thread(target=stream, daemon=True).start()
                elif cmd == "STOP":
                    scanning.clear()
                    conn.sendall(b"STOP OK\r\n")
                elif cmd.startswith("LIST"):
                    conn.sendall(f"NCHAN {NCHAN}\r\nFPS {FPS}\r\n".encode())
                elif cmd == "STATUS":
                    state = "SCAN" if scanning.is_set() else "READY"
                    conn.sendall(f"STATUS {state}\r\n".encode())
                else:
                    conn.sendall(b"ERR UNKNOWN\r\n")
    finally:
        scanning.clear()
        conn.close()
        print(f"[mock] client disconnected: {addr}")


def main() -> None:
    port = int(sys.argv[1]) if len(sys.argv) > 1 else DEFAULT_PORT
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
        s.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
        s.bind((HOST, port))
        s.listen(1)
        print(f"[mock] listening on {HOST}:{port}")
        while True:
            conn, addr = s.accept()
            threading.Thread(
                target=handle_client, args=(conn, addr), daemon=True
            ).start()


if __name__ == "__main__":
    main()
