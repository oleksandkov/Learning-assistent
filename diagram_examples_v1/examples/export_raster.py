"""Renders every generated SVG to PNG and JPEG.

    uv run --with cairosvg --with pillow python examples/export_raster.py

scale=3 gives print-grade raster output; scale=1.5 keeps files small.
JPEG cannot carry transparency, so blocks are composited onto white.
"""

from __future__ import annotations

import io
import pathlib
import sys

import cairosvg
from PIL import Image

HERE = pathlib.Path(__file__).resolve().parent
ROOT = HERE.parent
SCALE = 3.0
PAD = 12


def to_png(svg: pathlib.Path, dest: pathlib.Path, scale: float) -> tuple[int, int]:
    png = cairosvg.svg2png(
        url=str(svg), scale=scale, background_color=None, output_width=None
    )
    dest.write_bytes(png)
    with Image.open(io.BytesIO(png)) as im:
        return im.size


def to_jpeg(svg: pathlib.Path, dest: pathlib.Path, scale: float) -> tuple[int, int]:
    png = cairosvg.svg2png(url=str(svg), scale=scale, background_color=None)
    with Image.open(io.BytesIO(png)) as im:
        flat = Image.new("RGB", im.size, "#ffffff")
        flat.paste(im, mask=im.split()[-1] if im.mode == "RGBA" else None)
        flat.save(dest, "JPEG", quality=95, subsampling=0)
        return flat.size


if __name__ == "__main__":
    files = sorted(ROOT.glob("*.svg"))
    if not files:
        sys.exit("no SVG files found")
    for f in files:
        p = f.with_suffix(".png")
        j = f.with_suffix(".jpg")
        ps = to_png(f, p, SCALE)
        js = to_jpeg(f, j, SCALE)
        print(f"{f.name}  png {ps[0]}x{ps[1]}  jpg {js[0]}x{js[1]}")