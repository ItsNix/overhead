#!/usr/bin/env python3
"""Save the device's screen as PNG:  python3 tools/snap.py <host> [out.png]"""
import sys
import urllib.request

from PIL import Image

host = sys.argv[1]
out = sys.argv[2] if len(sys.argv) > 2 else "snap.png"
data = urllib.request.urlopen(f"http://{host}/snap", timeout=30).read()
img = Image.new("RGB", (800, 480))
px = img.load()
for i in range(800 * 480):
    v = data[2 * i] << 8 | data[2 * i + 1]   # RGB565, byte-swapped in the framebuffer
    px[i % 800, i // 800] = ((v >> 11 & 31) << 3, (v >> 5 & 63) << 2, (v & 31) << 3)
img.save(out)
print(out)
