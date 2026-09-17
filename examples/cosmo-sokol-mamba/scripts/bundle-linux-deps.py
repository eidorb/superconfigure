#!/usr/bin/env python3
"""Fetch conda-forge linux-64 X11/GL client libs into vendor/linux-64.

Uses the lock produced by APE micromamba's solver (see README). Extraction
of .conda is done here because APE micromamba currently writes its pkgs
cache as pkgs/https/conda.anaconda.org/... and fails extract.
"""
import io
import os
import shutil
import sys
import tarfile
import urllib.request
import zipfile
from pathlib import Path

FILES = [
    "xorg-libx11-1.8.13-he1eb515_1.conda",
    "xorg-libxau-1.0.12-hb03c661_2.conda",
    "xorg-libxcursor-1.2.3-hb9d3cd8_0.conda",
    "xorg-libxdmcp-1.1.5-hb03c661_2.conda",
    "xorg-libxext-1.3.7-h7cc23a3_1.conda",
    "xorg-libxfixes-6.0.2-h7cc23a3_1.conda",
    "xorg-libxi-1.8.3-h7cc23a3_1.conda",
    "xorg-libxrandr-1.5.5-h7cc23a3_1.conda",
    "xorg-libxrender-0.9.12-hb03c661_1.conda",
    "libxcb-1.17.0-hb83e432_2.conda",
    "pthread-stubs-0.4-h7cc23a3_1004.conda",
    "libgl-1.7.0-ha4b6fd6_5.conda",
    "libglvnd-1.7.0-ha4b6fd6_5.conda",
    "libglx-1.7.0-ha4b6fd6_5.conda",
    "libopengl-1.7.0-ha4b6fd6_5.conda",
]
BASE = "https://conda.anaconda.org/conda-forge/linux-64/"


def extract_conda(path: Path, dest_lib: Path) -> None:
    import zstandard as zstd

    with zipfile.ZipFile(path) as z:
        names = z.namelist()
        pkg = [n for n in names if n.startswith("pkg-") and n.endswith(".tar.zst")]
        if not pkg:
            pkg = [n for n in names if n.endswith(".tar.zst") and "info-" not in n]
        raw = z.read(pkg[0])
    tar_bytes = zstd.ZstdDecompressor().decompress(raw, max_output_size=512 << 20)
    with tarfile.open(fileobj=io.BytesIO(tar_bytes), mode="r:") as tar:
        for m in tar.getmembers():
            if not m.isfile():
                continue
            if "/lib/" not in "/" + m.name and not m.name.startswith("lib/"):
                continue
            bn = os.path.basename(m.name)
            if ".so" not in bn:
                continue
            src = tar.extractfile(m)
            out = dest_lib / bn
            out.write_bytes(src.read())
            out.chmod(0o755)


def main() -> None:
    try:
        import zstandard  # noqa: F401
    except ImportError:
        os.system(f"{sys.executable} -m pip install -q zstandard")

    root = Path(__file__).resolve().parents[1]
    dst = root / "vendor" / "linux-64"
    libdir = dst / "lib"
    if libdir.exists():
        shutil.rmtree(libdir)
    libdir.mkdir(parents=True)
    cache = Path("/tmp/conda-pkgs")
    cache.mkdir(exist_ok=True)
    for fn in FILES:
        local = cache / fn
        if not local.exists() or local.stat().st_size < 100:
            print("GET", fn)
            urllib.request.urlretrieve(BASE + fn, local)
        extract_conda(local, libdir)
    print("extracted", len(list(libdir.iterdir())), "files")


if __name__ == "__main__":
    main()
