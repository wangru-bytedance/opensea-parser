#!/usr/bin/env python3
"""CLI regression using K3109W0K_FARMC_20260916T000235.frmc (not bundled).

Run from the parent openSeaChest_LogParser checkout:
  python3 opensea-parser/tests/integration_farm_combined.py \
    --parser Make/gcc/openSeaChest_LogParser_x86_64 \
    --sample /path/to/K3109W0K_FARMC_20260916T000235.frmc \
    --output-dir farmfix_tmp/combined_regression

Optionally supply --baseline-parser and repeated --ordinary-log arguments to
compare unchanged standalone FARM output against the pre-change executable.
"""

import argparse
import json
from pathlib import Path
import struct
import subprocess


NAMES = [
    "Current FARM Log",
    "Factory FARM Log",
    "Host Saved FARM Disc Log",
    "Time Series FARM Log",
    "Long Term Saved FARM Log",
    "Sticky FARM Log",
]
SLOT_SIZE = 0x18000


def unique_object(pairs):
    result = {}
    for key, value in pairs:
        if key in result:
            raise AssertionError(f"Duplicate JSON key: {key}")
        result[key] = value
    return result


def run(parser, source, target, options=(), from_pipe=False):
    command = [
        str(parser), "--inputLog", str(source),
        "--logType", "farmLog", "--printType", "json", "--outputLog", str(target),
        *options,
    ]
    if from_pipe:
        # The existing CLI only enters its parser when the file flag is set.
        # The second option selects the buffer constructor while retaining that
        # flag. This tests the library buffer path, not standalone CLI pipe use.
        command += ["--inputLog", "fromPipe"]
    target.unlink(missing_ok=True)
    process = subprocess.run(
        command, input=source.read_bytes().hex(" ").encode() if from_pipe else None,
        capture_output=True, timeout=20,
    )
    assert process.returncode >= 0, (source.name, process.returncode, process.stderr)
    assert target.exists(), (source.name, process.stdout, process.stderr)
    document = json.loads(target.read_text(), object_pairs_hook=unique_object)
    document.pop("openSeaChest_LogParser", None)
    return process.returncode, document


def changed(source, offset, fmt, value):
    result = bytearray(source)
    struct.pack_into(fmt, result, offset, value)
    return result


def main():
    args = argparse.ArgumentParser(description=__doc__)
    args.add_argument("--parser", required=True, type=Path)
    args.add_argument("--sample", required=True, type=Path)
    args.add_argument("--output-dir", required=True, type=Path)
    args.add_argument("--baseline-parser", type=Path)
    args.add_argument("--ordinary-log", type=Path, action="append", default=[])
    options = args.parse_args()
    parser = options.parser.resolve()
    output = options.output_dir.resolve()
    output.mkdir(parents=True, exist_ok=True)
    sample = options.sample.read_bytes()
    assert sample[:16] == b"STX_FARM_COMBINE"
    count, stride = struct.unpack_from("<HH", sample, 252)
    assert (count, stride) == (7, 32), "This regression expects the documented K3109W0K fixture"
    entries = [struct.unpack_from("<8sIIQQ", sample, 256 + i * stride) for i in range(count)]
    assert [entry[0] for entry in entries] == [
        b"FARM    ", b"FACTORY ", b"FARMSAVE", b"FARMTIME",
        b"FARMLONG", b"FARMSTCK", b"WORKLDTC",
    ]
    source = output / "combined.frmc"
    source.write_bytes(sample)
    code, document = run(parser, source, output / "combined.json")
    assert code != 0, "Unrecognized WORKLDTC must not report complete success"
    assert document["FARM Combined Labels"] == {
        "serial_number": "K3109W0K",
        "firmware_rev": "SR02",
        "Drive Interface": "SATA",
        "Model Number": "ST36000NM001M-3UV133",
        "FARM Combined version": "1.1.0",
    }
    directory = document["FARM Combined Data Sets"]
    assert [entry["Parsing Status"] for entry in directory] == ["Success"] * 6 + ["Failed"]
    assert "Validation Error" in document["FARM WorkLoad Trace Log"][0]
    for actual, (tag, start, size, begin, end) in zip(directory, entries):
        assert actual["Type"] == tag.decode().rstrip()
        assert (actual["Offset"], actual["Size"]) == (start, size)
        assert actual["Start Time (ms since Unix epoch)"] == begin
        assert actual["End Time (ms since Unix epoch)"] == end

    # Compare every slot, including all empty slots, through both entry paths.
    comparisons = 0
    populated = empty = 0
    for mode, flags in [
        ("default", ()), ("status", ("--showStatusBits",)),
        ("static", ("--showStatic",)),
        ("status_static", ("--showStatusBits", "--showStatic")),
    ]:
        mode_code, combined = run(parser, source, output / f"{mode}.json", flags)
        assert mode_code == code
        for index, name in enumerate(NAMES):
            _, start, size, _, _ = entries[index]
            frames = combined[name] if index >= 3 else [combined[name]]
            assert len(frames) == size // SLOT_SIZE
            for slot, actual in enumerate(frames):
                raw = output / f"dataset_{index}_slot_{slot}.bin"
                raw.write_bytes(sample[start + slot * SLOT_SIZE:start + (slot + 1) * SLOT_SIZE])
                raw_code, expected = run(parser, raw, output / f"raw_{mode}_{index}_{slot}.json", flags)
                assert raw_code == 0, (mode, index, slot, raw_code)
                assert actual == expected, (mode, name, slot)
                comparisons += 1
                if mode == "default":
                    if "Empty FARM Log" in actual:
                        empty += 1
                    else:
                        populated += 1
    expected_empty = sum(
        sample[start + slot * SLOT_SIZE:start + slot * SLOT_SIZE + 7]
        in (bytes(7), b"\xff" * 7)
        for _, start, size, _, _ in entries[:6]
        for slot in range(size // SLOT_SIZE)
    )
    assert empty == expected_empty
    assert populated + empty == sum(entry[2] // SLOT_SIZE for entry in entries[:6])

    # A container without WORKLDTC succeeds; a single dataset is also valid.
    known = changed(sample, 252, "<H", 6)
    known = changed(known, 116, "<H", 256 + 6 * 32)
    source.write_bytes(known)
    known_code, known_doc = run(parser, source, output / "known_only.json")
    assert known_code == 0
    for name in NAMES:
        assert known_doc[name] == document[name]
    single = changed(known, 252, "<H", 1)
    single = changed(single, 116, "<H", 288)
    single = single[:entries[0][1] + entries[0][2]]
    source.write_bytes(single)
    single_code, single_doc = run(parser, source, output / "single.json")
    assert single_code == 0 and single_doc[NAMES[0]] == document[NAMES[0]]
    # The CLI's existing pipe limit is 99,000 bytes. One combined dataset fits.
    pipe_code, piped = run(parser, source, output / "pipe.json", from_pipe=True)
    assert (pipe_code, piped) == (single_code, single_doc)

    # FF-filled unused slots have the same empty-frame semantics as zero slots.
    ff_empty = bytearray(known)
    empty_offset = next(
        entries[3][1] + slot * SLOT_SIZE
        for slot in range(entries[3][2] // SLOT_SIZE)
        if sample[entries[3][1] + slot * SLOT_SIZE:entries[3][1] + slot * SLOT_SIZE + 7] == bytes(7)
    )
    ff_empty[empty_offset:empty_offset + SLOT_SIZE] = b"\xff" * SLOT_SIZE
    source.write_bytes(ff_empty)
    ff_code, ff_doc = run(parser, source, output / "ff_empty.json")
    assert ff_code == 0 and ff_doc[NAMES[3]] == document[NAMES[3]]

    malformed = {
        "short_header": sample[:128],
        "truncated_payload": sample[:-1],
        "zero_stride": changed(sample, 254, "<H", 0),
        "short_stride": changed(sample, 254, "<H", 16),
        "excess_count": changed(sample, 252, "<H", 0xFFFF),
        "no_datasets": changed(sample, 252, "<H", 0),
        "offset_in_header": changed(sample, 264, "<I", 0),
        "offset_past_end": changed(sample, 264, "<I", 0xFFFFFFFF),
        "overflow_range": changed(sample, 268, "<I", 0xFFFFFFFF),
        "empty_dataset": changed(sample, 268, "<I", 0),
        "overlapping_datasets": changed(sample, 296, "<I", entries[0][1]),
        "zero_page_size": changed(known, entries[0][1] + 40, "<Q", 0),
        "huge_copy_count": changed(known, entries[0][1] + 56, "<Q", 0xC0FFFFFFFFFFFFFF),
        "short_frame": changed(known, entries[0][1] + 32, "<Q", 0xC000000000004000),
        "huge_frame": changed(known, entries[0][1] + 32, "<Q", 0xC0FFFFFFFFFFFFFF),
        "bad_signature": changed(known, entries[0][1], "<Q", 12345),
    }
    for name, payload in malformed.items():
        source.write_bytes(payload)
        error_code, error_doc = run(parser, source, output / f"invalid_{name}.json")
        assert error_code != 0, (name, error_code)
        if name in {"zero_page_size", "huge_copy_count", "short_frame", "huge_frame", "bad_signature"}:
            assert error_doc["FARM Combined Data Sets"][0]["Parsing Status"] == "Failed"
            assert error_doc[NAMES[1]] == document[NAMES[1]], "Later datasets should still parse"
    source.write_bytes(sample)

    standalone = 0
    if options.baseline_parser:
        for index, raw in enumerate(options.ordinary_log):
            before_code, before = run(options.baseline_parser.resolve(), raw.resolve(), output / f"before_{index}.json")
            after_code, after = run(parser, raw.resolve(), output / f"after_{index}.json")
            assert (after_code, after) == (before_code, before), raw.name
            standalone += 1
    result = {
        "frame_comparisons": comparisons,
        "populated_frames": populated,
        "empty_slots": empty,
        "malformed_inputs_rejected": len(malformed),
        "ordinary_logs_unchanged": standalone,
        "workload_trace": "Restored WLM decoder rejects incompatible HLWN layout with diagnostics",
        "buffer_input": "Single-dataset container matches file input (CLI file-flag workaround)",
    }
    (output / "verification.json").write_text(json.dumps(result, indent=2) + "\n")
    print(json.dumps(result, indent=2))


if __name__ == "__main__":
    main()
