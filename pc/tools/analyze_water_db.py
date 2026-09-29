#!/usr/bin/env python3
"""Read-only analyzer for assets/Data/water.db (SQLite).

Does not modify the database. Works on Windows/Linux/macOS with Python 3.

Usage:
  python pc/tools/analyze_water_db.py path/to/assets/Data/water.db
"""

from __future__ import annotations

import argparse
import os
import sqlite3
import struct
import sys


def header_info(path: str) -> None:
    with open(path, "rb") as f:
        raw = f.read(100)
    print(f"size_bytes: {os.path.getsize(path)}")
    print(f"header_hex: {raw[:16].hex()}")
    print(f"header_ascii: {raw[:16]!r}")
    if raw.startswith(b"SQLite format 3\x00"):
        print("format: SQLite 3 [CONFIRMED]")
        # SQLite header fields (https://www.sqlite.org/fileformat.html)
        page_size = struct.unpack(">H", raw[16:18])[0]
        if page_size == 1:
            page_size = 65536
        write_ver = raw[18]
        read_ver = raw[19]
        print(f"page_size: {page_size}")
        print(f"write_version: {write_ver}  read_version: {read_ver}")
    else:
        print("format: NOT SQLite header [UNKNOWN proprietary?]")


def analyze(path: str) -> int:
    if not os.path.isfile(path):
        print(f"error: file not found: {path}", file=sys.stderr)
        return 1

    print("=== HEADER ===")
    header_info(path)

    uri = f"file:{os.path.abspath(path)}?mode=ro"
    con = sqlite3.connect(uri, uri=True)
    cur = con.cursor()

    print("\n=== SQLITE_MASTER ===")
    cur.execute(
        "SELECT type, name, sql FROM sqlite_master WHERE name NOT LIKE 'sqlite_%' ORDER BY type, name"
    )
    rows = cur.fetchall()
    for typ, name, sql in rows:
        print(f"\n[{typ}] {name}")
        if sql:
            print(sql)

    print("\n=== ROW COUNTS ===")
    tables = [n for t, n, _ in rows if t == "table"]
    for name in tables:
        cur.execute(f'SELECT COUNT(*) FROM "{name}"')
        print(f"  {name}: {cur.fetchone()[0]}")

    if "Settings" in tables:
        print("\n=== Settings ===")
        cur.execute("SELECT Name, Value FROM Settings ORDER BY Name")
        for n, v in cur.fetchall():
            print(f"  {n} = {v}")

    if "LevelInfo" in tables:
        print("\n=== LevelInfo columns ===")
        cur.execute('PRAGMA table_info("LevelInfo")')
        cols = [r[1] for r in cur.fetchall()]
        print(" ", cols)
        cur.execute("SELECT COUNT(*) FROM LevelInfo")
        print(f"  total levels: {cur.fetchone()[0]}")
        # sample 01_CUT_FIRST if present
        cur.execute(
            "SELECT ID, Name, Filename, PackName, ParTime, Unlocked "
            "FROM LevelInfo WHERE Filename LIKE '%CUT_FIRST%' OR Name LIKE '%CUT_FIRST%'"
        )
        for r in cur.fetchall():
            print("  match:", dict(zip(["ID", "Name", "Filename", "PackName", "ParTime", "Unlocked"], r)))

    # Evidence note: no fluid/physics tables
    print("\n=== PHYSICS / FLUID TABLE SEARCH ===")
    hits = []
    for typ, name, sql in rows:
        blob = (sql or "").lower() + " " + name.lower()
        if any(k in blob for k in ("particle", "fluid", "phys", "collis", "spout", "emitter")):
            hits.append(name)
    if hits:
        print("  possible:", hits)
    else:
        print("  none — water.db is progress/catalog, not fluid simulation data [CONFIRMED by schema]")

    con.close()
    return 0


def main() -> int:
    ap = argparse.ArgumentParser(description="Analyze WMW water.db (read-only)")
    ap.add_argument("db_path", help="Path to water.db")
    args = ap.parse_args()
    return analyze(args.db_path)


if __name__ == "__main__":
    sys.exit(main())
