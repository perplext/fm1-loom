#!/usr/bin/env python3
"""the image's sections and its largest symbols, from the toolchain's objdump: size_report.py OBJDUMP ELF [MIN]

A size report for fitting the image into the app slot (the release workflow prints it after a build)."""
import re
import subprocess
import sys

od, elf = sys.argv[1], sys.argv[2]
least = int(sys.argv[3]) if len(sys.argv) > 3 else 96
for l in subprocess.run([od, "-h", elf], capture_output=True, text=True).stdout.splitlines():
    f = l.split()
    if len(f) >= 4 and f[1].startswith(".") and re.fullmatch("[0-9a-f]{8}(?:[0-9a-f]{8})?", f[2]):
        print("%-24s %8d" % (f[1], int(f[2], 16)))
rows = []
for l in subprocess.run([od, "-t", elf], capture_output=True, text=True).stdout.splitlines():
    f = l.split()
    if len(f) < 5:
        continue
    for i in range(2, len(f) - 1):
        if re.fullmatch("[0-9a-f]{8}(?:[0-9a-f]{8})?", f[i]):
            if int(f[i], 16) >= least:
                rows.append((int(f[i], 16), f[i - 1], f[-1]))
            break
for sz, sec, nm in sorted(rows, reverse=True)[:260]:
    print("%7d %-16s %s" % (sz, sec, nm))
