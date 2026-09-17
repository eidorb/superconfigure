# micromamba 2.9.0 (Cosmopolitan / superconfigure)

Fat APE target: `make LOG=stdout micromamba`  
Result: `results/bin/micromamba.com`

Verified here:

- `--version` → `2.9.0` on x86_64, aarch64/qemu, and the fat APE
- `info` reports OpenSSL 3.6.2, curl 8.10.1, libarchive 3.8.0, nghttp2, brotli, zstd
- `search xz -c conda-forge` succeeds (TLS + solver) on the fat binary
- conda platform is **runtime**: Cosmopolitan `IsLinux()` / `IsXnu()` / `IsWindows()` pick
  `linux-*` / `osx-*` / `win-*`; arch follows the running APE slice (`x86_64` vs `aarch64`),
  except Windows ARM: `IsWow64Process2` native machine `0xAA64` → `win-arm64` (same as
  native micromamba). Override with `CONDA_SUBDIR` / `MAMBA_PLATFORM` as usual.

## krb5 / GSSAPI — intentionally omitted

Upstream libmamba `REQUIRED_STATIC_DEPS` lists `libgssapi_krb5.a` and the rest of MIT krb5 because **conda-forge's static curl is GSSAPI-enabled**. Superconfigure curl is not:

```
--with-openssl --with-nghttp2   (no --with-gssapi)
features: SSL HTTP2 PSL NTLM brotli zstd   (no GSS-API / Kerberos / SPNEGO)
```

There is no `lib/krb5` recipe; OpenSSH is already `--without-kerberos5`. The micromamba patch drops those five `.a` files and adds the *actual* curl/archive transitive static libs this prefix provides: brotli, expat, pcre2.

Lost: Negotiate auth to a KDC-backed channel/proxy. Not needed for conda-forge HTTPS.

To match official static micromamba later: port MIT krb5, rebuild curl `--with-gssapi`, restore the five libs in `minimal.diff`.

## Other Cosmopolitan patches in `minimal.diff`

- Runtime host OS via APE `__hostos` (`build.hpp` `on_linux`/`on_mac`/`on_win`, `build_platform()`)
- `if constexpr (util::on_win)` sites switched to runtime `if` (not constexpr on Cosmo)
- Do not pass `-static-libstdc++ -static-libgcc` (cosmocc)
- Drop `rt` / `dl` from `MAMBA_FORCE_DYNAMIC_LIBS`
- `dladdr` is missing in cosmocc 3.9.2 → `get_self_exe_path()`
- `std::shared_mutex` + constinit hangs in `pthread_delay_np` on Cosmo; both `logging.cpp` and `singletons.cpp` must use `std::mutex` (ODR; mismatch = infinite spin on `--version`)
- `__glibc` via host `getconf GNU_LIBC_VERSION` (Cosmo `confstr` is empty). Still overridable with `CONDA_OVERRIDE_GLIBC`

## Dependency recipes added

`fmt` (header-only, `FMT_OS=OFF`), `spdlog` (`SPDLOG_FMT_EXTERNAL`), `json`, `expected`, `simdjson`, `yaml-cpp`, `reproc`, `msgpack-c`, `nghttp2`, `libsolv`, `cli11`.

Also: include `compress/xz` in `compress/BUILD.mk`, xz URL → GitHub, curl now depends on nghttp2, `check_sha.sh` uses `--no-same-owner`.

Notable dep patches:

- simdjson: include `<cmath>` before `<cfloat>` (Cosmo `float.h` pulls `math.h` and poisons libcxx); skip `-mno-avx256-*` when the compiler name contains `aarch64`
- libsolv: GNU `qsort_r` via `__COSMOPOLITAN__`
- reproc: Cosmo rejects `extern const int X = -EINVAL` at constexpr-init time → constructor-initialized non-const
- spdlog `tweakme.h`: `#define SPDLOG_FMT_EXTERNAL`

## Local aarch64 note

autotools `configure` for aarch64 needs qemu-user binfmt so test programs can run:

```
qemu-aarch64-static  +  binfmt registration for ELF aarch64
```

Without it, curl/nghttp2/xz/etc. `configure` fail looking for a working C compiler.

## Known leftovers

- Fat `micromamba info` may print **ape** as the binary name: `get_self_exe_path().stem()` is `/usr/bin/ape` under binfmt, not `micromamba.com`
- `__glibc` virtual package comes from host `getconf GNU_LIBC_VERSION` on Linux. Override with `CONDA_OVERRIDE_GLIBC` if needed.
- Binaries are unstripped; zip overlay ships CA certs, not `openssl.cnf`. conda-forge TLS still worked
