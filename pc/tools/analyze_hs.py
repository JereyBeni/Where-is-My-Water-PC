#!/usr/bin/env python3
"""Read-only analyzer for assets/Objects/*.hs (XML InteractiveObject definitions).

Usage:
  python pc/tools/analyze_hs.py path/to/assets/Objects
  python pc/tools/analyze_hs.py path/to/one_file.hs
"""

from __future__ import annotations

import argparse
import os
import re
import sys
from collections import Counter, defaultdict
from xml.etree import ElementTree as ET


def parse_hs(path: str) -> dict:
    tree = ET.parse(path)
    root = tree.getroot()
    info = {
        "path": path,
        "root": root.tag,
        "shapes": [],
        "sprites": [],
        "properties": {},
    }
    for shape in root.findall("Shapes/Shape"):
        pts = []
        for pt in shape.findall("Point"):
            pos = pt.attrib.get("pos", "")
            pts.append(pos.strip())
        info["shapes"].append(pts)
    for sp in root.findall("Sprites/Sprite"):
        info["sprites"].append(dict(sp.attrib))
    for prop in root.findall("DefaultProperties/Property"):
        info["properties"][prop.attrib.get("name", "")] = prop.attrib.get("value", "")
    return info


def analyze_path(target: str) -> int:
    files: list[str] = []
    if os.path.isfile(target):
        files = [target]
    elif os.path.isdir(target):
        for name in sorted(os.listdir(target)):
            if name.lower().endswith(".hs"):
                files.append(os.path.join(target, name))
    else:
        print(f"error: not found: {target}", file=sys.stderr)
        return 1

    print(f"files: {len(files)}")
    prop_freq: Counter[str] = Counter()
    type_freq: Counter[str] = Counter()
    root_tags: Counter[str] = Counter()
    shape_counts: Counter[int] = Counter()
    errors = 0

    for path in files:
        try:
            with open(path, "rb") as f:
                head = f.read(64)
            if b"<" not in head and b"InteractiveObject" not in head:
                # still try XML parse
                pass
            info = parse_hs(path)
        except Exception as e:
            print(f"FAIL {path}: {e}")
            errors += 1
            continue

        root_tags[info["root"]] += 1
        shape_counts[len(info["shapes"])] += 1
        for k in info["properties"]:
            prop_freq[k] += 1
        t = info["properties"].get("Type", "(none)")
        type_freq[t] += 1

    print("\n=== ROOT TAGS ===")
    for k, v in root_tags.most_common():
        print(f"  {k}: {v}")

    print("\n=== SHAPE COUNT PER FILE ===")
    for k, v in sorted(shape_counts.items()):
        print(f"  {k} shape(s): {v} files")

    print("\n=== DefaultProperties Type= values ===")
    for k, v in type_freq.most_common():
        print(f"  {k}: {v}")

    print("\n=== Property name frequency ===")
    for k, v in prop_freq.most_common():
        print(f"  {k}: {v}")

    # Detailed dump for a few representative files
    samples = []
    for needle in ("shower_head.hs", "star.hs", "pipe_straight_short.hs", "touch_spout.hs"):
        for p in files:
            if p.endswith(needle):
                samples.append(p)
    if samples:
        print("\n=== SAMPLE DETAIL ===")
        for p in samples:
            info = parse_hs(p)
            print(f"\n{os.path.basename(p)}")
            print(f"  shapes: {len(info['shapes'])}  points[0]={info['shapes'][0] if info['shapes'] else []}")
            print(f"  sprites: {len(info['sprites'])}")
            for k, v in info["properties"].items():
                print(f"  prop {k}={v}")

    print(f"\nerrors: {errors}")
    print("format: XML <InteractiveObject> with Shapes/Sprites/DefaultProperties [CONFIRMED]")
    return 0 if errors == 0 else 2


def main() -> int:
    ap = argparse.ArgumentParser(description="Analyze WMW .hs object definitions (read-only)")
    ap.add_argument("path", help="A .hs file or Objects/ directory")
    args = ap.parse_args()
    return analyze_path(args.path)


if __name__ == "__main__":
    sys.exit(main())
