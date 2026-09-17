#!/usr/bin/env python3
"""Lock environment.yml per conda subdir with APE micromamba, download, unpack.

APE micromamba can *solve* but cannot *extract*: its pkgs cache becomes
  $CONDA_PKGS_DIRS/https/conda.anaconda.org/...
and extract dies with `ape error: package: not found`. So we take the
dry-run FETCH urls and unpack the .conda files ourselves.
"""
from __future__ import annotations

import io
import json
import os
import shutil
import subprocess
import sys
import tarfile
import urllib.request
import zipfile
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
VENDOR = ROOT / "vendor"
ENV_YML = ROOT / "environment.yml"
CACHE = Path(os.environ.get("CONDA_PKGS_DIRS", "/tmp/mamba-ape-pkgs"))
APE = Path(
    os.environ.get(
        "APE",
        "/workspace/superconfigure/cosmopolitan/.cosmocc/3.9.2/bin/ape-x86_64.elf",
    )
)
MAMBA = Path(
    os.environ.get(
        "MAMBA",
        "/workspace/superconfigure/results/bin/micromamba.com",
    )
)
SUBDIRS = [
    "linux-64",
    "linux-aarch64",
    "osx-64",
    "osx-arm64",
    "win-64",
    "win-arm64",
]
KEEP_PREFIXES = ("lib/", "Library/", "bin/")
KEEP_BASENAME = ("libz", "zlib")


def ensure_zstd():
    try:
        import zstandard  # noqa: F401
    except ImportError:
        subprocess.check_call([sys.executable, "-m", "pip", "install", "-q", "zstandard"])


def micromamba_fetch(subdir: str) -> list[dict]:
    env = os.environ.copy()
    env["CONDA_SUBDIR"] = subdir
    env["CONDA_OVERRIDE_GLIBC"] = env.get("CONDA_OVERRIDE_GLIBC", "2.28")
    env["CONDA_OVERRIDE_OSX"] = env.get("CONDA_OVERRIDE_OSX", "13.0")
    env["MAMBA_NO_BANNER"] = "1"
    # dummy prefix; --dry-run does not write it
    prefix = f"/tmp/mamba-dry-{subdir}"
    cmd = [
        str(APE),
        str(MAMBA),
        "create",
        "--dry-run",
        "--json",
        "-y",
        "-p",
        prefix,
        "-f",
        str(ENV_YML),
    ]
    raw = subprocess.check_output(cmd, env=env, stderr=subprocess.STDOUT)
    # micromamba prints progress on stdout before JSON
    text = raw.decode("utf-8", "replace")
    start = text.find("{")
    if start < 0:
        raise SystemExit(f"no JSON from micromamba for {subdir}:\n{text[-500:]}")
    data = json.loads(text[start:])
    if not data.get("success", True) and "actions" not in data:
        raise SystemExit(f"solve failed for {subdir}: {data}")
    return data.get("actions", {}).get("FETCH", [])


def extract_conda(archive: Path, dest: Path) -> int:
    import zstandard as zstd

    with zipfile.ZipFile(archive) as z:
        names = z.namelist()
        pkg = [n for n in names if n.startswith("pkg-") and n.endswith(".tar.zst")]
        if not pkg:
            pkg = [n for n in names if n.endswith(".tar.zst") and not n.startswith("info-")]
        raw = z.read(pkg[0])
    blob = zstd.ZstdDecompressor().decompress(raw, max_output_size=512 << 20)
    n = 0
    with tarfile.open(fileobj=io.BytesIO(blob), mode="r:") as tar:
        for m in tar.getmembers():
            if not m.isfile():
                continue
            name = m.name.lstrip("./")
            if not any(name.startswith(p) or f"/{p}" in f"/{name}" for p in KEEP_PREFIXES):
                continue
            bn = os.path.basename(name).lower()
            if not any(bn.startswith(k) for k in KEEP_BASENAME):
                continue
            # strip a leading <pkg>/ if present; keep lib/ and Library/
            rel = name
            for marker in ("/lib/", "/Library/", "/bin/"):
                idx = f"/{name}".find(marker)
                if idx >= 0:
                    rel = f"/{name}"[idx + 1 :]
                    break
            out = dest / rel
            out.parent.mkdir(parents=True, exist_ok=True)
            out.write_bytes(tar.extractfile(m).read())
            out.chmod(0o755)
            n += 1
    return n


def main() -> None:
    ensure_zstd()
    if not MAMBA.is_file():
        raise SystemExit(f"micromamba APE not found: {MAMBA}")
    CACHE.mkdir(parents=True, exist_ok=True)
    if VENDOR.exists():
        shutil.rmtree(VENDOR)
    VENDOR.mkdir()

    lock = {}
    for subdir in SUBDIRS:
        print(f"==> solve {subdir}")
        fetch = micromamba_fetch(subdir)
        dest = VENDOR / subdir
        dest.mkdir()
        pkgs = []
        for rec in fetch:
            url = rec["url"]
            fn = rec["fn"]
            local = CACHE / subdir / fn
            local.parent.mkdir(parents=True, exist_ok=True)
            if not local.exists() or local.stat().st_size < 64:
                print("    GET", fn)
                urllib.request.urlretrieve(url, local)
            n = extract_conda(local, dest)
            print(f"    {fn}: {n} files")
            pkgs.append({"fn": fn, "sha256": rec.get("sha256"), "url": url})
        lock[subdir] = pkgs

    (VENDOR / "lock.json").write_text(json.dumps(lock, indent=2) + "\n")
    print("vendor ready:", VENDOR)


if __name__ == "__main__":
    main()
