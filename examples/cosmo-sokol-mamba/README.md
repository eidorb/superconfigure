# cosmo-sokol-mamba

[cosmo-sokol](https://github.com/bullno1/cosmo-sokol) (Sokol + Dear ImGui APE)
on **cosmocc 4.0.2**, with conda-forge X11/GL client libraries in the zip
overlay.

On first Linux run the payload is copied to **`<exe-dir>/deps/lib/`**
(next to the binary, not XDG). `cosmo_dlopen` then loads those `.so` files.
Windows uses Cosmo NT + D3D11 (system DLLs). cosmocc 4.0.2 still does not
IAT-import Sokol's extra user32/gdi32/shell32 calls; [nt_imports.c](nt_imports.c)
resolves them with `GetProcAddress`. Host X server / GPU remain required.

```
export PATH=/workspace/cosmocc-4.0.2/bin:$PATH
./build
xvfb-run ./bin/cosmo-sokol-mamba.com
```

APE micromamba solved the X11/GL spec (`CONDA_OVERRIDE_GLIBC=2.28`) but
cannot extract `.conda` (cache path `pkgs/https/conda.anaconda.org/...`).
The files in `vendor/linux-64` were unpacked with Python. Conda RPATH is
`$ORIGIN/.`. Do not add conda libgcc; host `libc.so.6` satisfies `DT_NEEDED`.
