#!/usr/bin/env python3
"""Exercise restored WLM decoding through the CLI, using synthetic legacy frames.

These fixtures verify the historical v3/v6 code path; they are not evidence
for the undocumented HLWN layout found in current drive samples.
"""

import argparse
import json
from pathlib import Path
import struct

from integration_farm_combined import run


SLOT = 0x80000
HEADER = 0x3C
FOOTER = b"\x08FTW"
NAME = "FARM WorkLoad Trace Log"


def frame(revision, number, commands, footer=FOOTER):
    data = bytearray(SLOT)
    data[:4] = b"<HTW"
    struct.pack_into("<HHQQIHHIIIII", data, 4, number, revision, 100, 200,
                     0, 7, 0, HEADER + len(commands), 1, 3, 1, 1)
    data[HEADER:HEADER + len(commands)] = commands
    data[-8:-4] = footer
    return data


def container(trace):
    data = bytearray(512)
    data[:16] = b"STX_FARM_COMBINE"
    struct.pack_into("<HHH", data, 18, 0, 1, 1)
    data[24:28] = b"SATA"
    struct.pack_into("<H", data, 116, 288)
    struct.pack_into("<HH", data, 252, 1, 32)
    struct.pack_into("<8sIIQQ", data, 256, b"WORKLDTC", len(data), len(trace), 0, 0)
    return data + trace


def modified(data, offset, fmt, value):
    result = bytearray(data)
    struct.pack_into(fmt, result, offset, value)
    return result


def main():
    ap = argparse.ArgumentParser(description=__doc__)
    ap.add_argument("--parser", type=Path, required=True)
    ap.add_argument("--output-dir", type=Path, required=True)
    options = ap.parse_args()
    parser = options.parser.resolve()
    output = options.output_dir.resolve()
    output.mkdir(parents=True, exist_ok=True)
    commands = (
        b"\x30" + (0x123456).to_bytes(5, "little")  # Read, 8 blocks, absolute LBA
        + b"\x8c"                                 # Write, previous length, sequential
        + b"\x95\x0c\x02"                         # Read, 12 blocks, LBA minus 2
        + b"\xa2\x00\x02\x00\x01"                 # Read, 512 blocks, LBA plus 256
        + b"\xd0" + (1000).to_bytes(5, "little")
        + b"\xe0" + (1010).to_bytes(5, "little")
        + b"\xf0"
    )
    expected = [
        {"Command": "Read", "Transfer size": 8, "LBA": "0x0000123456"},
        {"Command": "Write", "Transfer size": 8, "LBA": "0x000012345E"},
        {"Command": "Read", "Transfer size": 12, "LBA": "0x000012345C"},
        {"Command": "Read", "Transfer size": 512, "LBA": "0x000012355C"},
        {"Command": "Streaming Start", "LBA": "0x00000003E8"},
        {"Command": "Streaming End", "LBA": "0x00000003F2"},
        {"Command": "One Second Marker"},
    ]
    trace = frame(3, 42, commands) + frame(6, 43, commands, FOOTER[::-1])
    for wrapped in (False, True):
        name = "combined" if wrapped else "standalone"
        source = output / f"{name}.bin"
        source.write_bytes(container(trace) if wrapped else trace)
        code, doc = run(parser, source, output / f"{name}.json")
        assert code == 0, (name, code, doc)
        actual = doc[NAME]
        assert len(actual) == 2
        for index, decoded in enumerate(actual):
            assert decoded["version"] == str((3, 6)[index])
            assert decoded["Workload Frame Number"] == 42 + index
            assert decoded["Start Timestamp"] == 100
            assert decoded["End Timestamp"] == 200
            assert decoded["Frame Size"] == HEADER + len(commands)
            assert decoded["Trace Data"] == expected, decoded

    malformed = {
        "truncated_slot": trace[:-1],
        "short_header_size": modified(trace, 32, "<I", 59),
        "oversized_frame": modified(trace, 32, "<I", SLOT),
        "bad_revision": modified(trace, 6, "<H", 7),
        "bad_second_header": modified(trace, SLOT, "<I", 0),
        "bad_second_footer": modified(trace, 2 * SLOT - 8, "<I", 0),
        "truncated_absolute_lba": frame(3, 1, b"\x30\x01"),
        "truncated_transfer_length": frame(3, 1, b"\xa0\x01"),
        "truncated_delta": frame(6, 1, b"\x33\x01"),
        "reserved_length": frame(6, 1, b"\xb4"),
    }
    for name, payload in malformed.items():
        source = output / f"{name}.frmc"
        source.write_bytes(container(payload))
        code, _ = run(parser, source, output / f"{name}.json")
        assert code != 0, name

    # Exact record boundary must not consume an extra command from zero padding.
    empty = output / "empty_legacy.frmc"
    empty.write_bytes(container(frame(6, 0, b"")))
    code, doc = run(parser, empty, output / "empty_legacy.json")
    assert code == 0 and doc[NAME][0]["Trace Data"] == []

    result = {
        "legacy_revisions": [3, 6],
        "input_paths": ["standalone", "combined"],
        "commands_checked": len(expected) * 4,
        "footer_byte_orders": 2,
        "malformed_inputs_rejected": len(malformed),
        "exact_record_boundary": "passed",
        "fixture_origin": "synthetic, based on restored historical decoder",
    }
    (output / "verification.json").write_text(json.dumps(result, indent=2) + "\n")
    print(json.dumps(result, indent=2))


if __name__ == "__main__":
    main()
