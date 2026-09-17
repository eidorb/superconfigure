# mamba-ape-bundle

Worked example: **micromamba locks `environment.yml` for every conda-forge
subdir**, those packages are zipped into an APE, and a tiny Cosmo program
extracts **the host’s subdir next to the binary** (`<exe>/deps/`) then
`cosmo_dlopen`s zlib.

```
make            # bundle + compile + zip payload
make test       # ./bin/hello.com  →  zlibVersion 1.3.2
```

| step | what |
|---|---|
| [environment.yml](environment.yml) | conda-forge `zlib` (same spec, all platforms) |
| `make bundle` | APE micromamba `--dry-run --json` per `CONDA_SUBDIR`, download `.conda`, unpack `lib/` / `Library/` into `vendor/<subdir>/` |
| `make program` | `cosmocc` → `bin/hello.com`, then `zip` each subdir into the APE |
| runtime | `IsLinux` / `IsXnu` / `IsWindows` (+ WoA `IsWow64Process2`) pick `linux-64` / `osx-arm64` / `win-64` / `win-arm64` / … Extract `/zip/<subdir>` → `<exe-dir>/deps`, dlopen `libz` |

APE micromamba can solve but **cannot extract** (pkgs cache becomes
`…/https/conda.anaconda.org/…`, then `ape error: package: not found`).
The bundle script is the workaround: solver from micromamba, unpack in Python.

```
$ make test
host subdir linux-64
extracted 3 files from linux-64 -> bin/deps
dlopen bin/deps/lib/libz.so.1.3.2
zlibVersion 1.3.2
```

`make bundle` solves all six subdirs (`linux-64`, `linux-aarch64`, `osx-64`,
`osx-arm64`, `win-64`, `win-arm64`). Windows VC/UCRT packages are solved but
not packed — same idea as not shipping conda `libgcc` on Linux.

Python cannot `exec` the APE (`Exec format error` without binfmt); the bundle
script runs it via `ape-x86_64.elf`.
