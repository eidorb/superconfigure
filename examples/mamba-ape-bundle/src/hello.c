/* Tiny APE: extract conda-forge zlib for this host next to the binary, dlopen it. */
#include <cosmo.h>
#include <libc/dce.h>
#include <dlfcn.h>
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <sys/stat.h>
#include <unistd.h>

#if defined(__x86_64__)
#include <libc/nt/dll.h>
#endif

#ifndef _COSMO_SOURCE
#define _COSMO_SOURCE
#endif

#define PMAX 4096

static int windows_host_is_arm64(void) {
#if defined(__x86_64__)
    typedef int (*Wow64Fn)(intptr_t, uint16_t *, uint16_t *)
        __attribute__((ms_abi));
    intptr_t k32 = GetModuleHandle("kernel32.dll");
    if (!k32) {
        return 0;
    }
    Wow64Fn fn = (Wow64Fn)GetProcAddress(k32, "IsWow64Process2");
    if (!fn) {
        return 0;
    }
    uint16_t process = 0, native = 0;
    if (!fn(-1, &process, &native)) {
        return 0;
    }
    return native == 0xAA64;
#else
    return 0;
#endif
}

static const char *conda_subdir(void) {
    if (IsWindows()) {
#if defined(__aarch64__)
        return "win-arm64";
#else
        return windows_host_is_arm64() ? "win-arm64" : "win-64";
#endif
    }
    if (IsXnu()) {
#if defined(__aarch64__)
        return "osx-arm64";
#else
        return "osx-64";
#endif
    }
#if defined(__aarch64__)
    return "linux-aarch64";
#else
    return "linux-64";
#endif
}

static void dirname_of(const char *path, char *out, size_t n) {
    snprintf(out, n, "%s", path && path[0] ? path : ".");
    char *s = strrchr(out, '/');
    char *b = strrchr(out, '\\');
    if (b && (!s || b > s)) {
        s = b;
    }
    if (!s) {
        snprintf(out, n, ".");
        return;
    }
    *s = 0;
}

static int mkdir_p(const char *path) {
    char tmp[PMAX];
    snprintf(tmp, sizeof(tmp), "%s", path);
    for (char *p = tmp + 1; *p; p++) {
        if (*p == '/' || *p == '\\') {
            char c = *p;
            *p = 0;
            if (mkdir(tmp, 0755) && errno != EEXIST) {
                return -1;
            }
            *p = c;
        }
    }
    if (mkdir(tmp, 0755) && errno != EEXIST) {
        return -1;
    }
    return 0;
}

static int copy_file(const char *src, const char *dst) {
    char dir[PMAX];
    dirname_of(dst, dir, sizeof(dir));
    mkdir_p(dir);
    int in = open(src, O_RDONLY);
    int out = open(dst, O_WRONLY | O_CREAT | O_TRUNC, 0755);
    if (in < 0 || out < 0) {
        if (in >= 0) close(in);
        if (out >= 0) close(out);
        return -1;
    }
    char buf[65536];
    ssize_t n;
    while ((n = read(in, buf, sizeof(buf))) > 0) {
        char *p = buf;
        while (n > 0) {
            ssize_t w = write(out, p, (size_t)n);
            if (w < 0) {
                close(in);
                close(out);
                return -1;
            }
            n -= w;
            p += w;
        }
    }
    close(in);
    close(out);
    return 0;
}

static int extract_tree(const char *srcroot, const char *dstroot) {
    DIR *d = opendir(srcroot);
    if (!d) {
        return -1;
    }
    int n = 0;
    struct dirent *e;
    while ((e = readdir(d))) {
        if (e->d_name[0] == '.') {
            continue;
        }
        char from[PMAX], to[PMAX];
        snprintf(from, sizeof(from), "%s/%s", srcroot, e->d_name);
        snprintf(to, sizeof(to), "%s/%s", dstroot, e->d_name);
        struct stat st;
        if (stat(from, &st) != 0) {
            continue;
        }
        if (S_ISDIR(st.st_mode)) {
            mkdir_p(to);
            int k = extract_tree(from, to);
            if (k > 0) {
                n += k;
            }
        } else {
            if (copy_file(from, to) == 0) {
                n++;
            }
        }
    }
    closedir(d);
    return n;
}

static int extract_deps(const char *subdir, char *depsdir, size_t n) {
    char exedir[PMAX];
    dirname_of(GetProgramExecutableName(), exedir, sizeof(exedir));
    snprintf(depsdir, n, "%s/deps", exedir);
    char marker[PMAX];
    snprintf(marker, sizeof(marker), "%s/.subdir", depsdir);
    FILE *mf = fopen(marker, "r");
    if (mf) {
        char have[64] = {0};
        fgets(have, sizeof(have), mf);
        fclose(mf);
        if (strncmp(have, subdir, strlen(subdir)) == 0) {
            return 0;
        }
    }
    mkdir_p(depsdir);
    char ziproot[PMAX];
    snprintf(ziproot, sizeof(ziproot), "/zip/%s", subdir);
    int got = extract_tree(ziproot, depsdir);
    if (got <= 0) {
        char fallback[PMAX];
        snprintf(fallback, sizeof(fallback), "vendor/%s", subdir);
        got = extract_tree(fallback, depsdir);
    }
    if (got <= 0) {
        fprintf(stderr, "no payload for %s (zip %s)\n", subdir, ziproot);
        return -1;
    }
    mf = fopen(marker, "w");
    if (mf) {
        fprintf(mf, "%s\n", subdir);
        fclose(mf);
    }
    printf("extracted %d files from %s -> %s\n", got, subdir, depsdir);
    return 0;
}

static void *open_zlib(const char *depsdir) {
    static const char *names[] = {
        "lib/libz.so.1",
        "lib/libz.so.1.3.2",
        "lib/libz.so",
        "lib/libz.1.dylib",
        "lib/libz.dylib",
        "Library/bin/zlib.dll",
        "Library/lib/zlib.dll",
        "bin/zlib.dll",
        NULL,
    };
    for (int i = 0; names[i]; i++) {
        char path[PMAX];
        snprintf(path, sizeof(path), "%s/%s", depsdir, names[i]);
        void *h = cosmo_dlopen(path, RTLD_NOW);
        if (h) {
            printf("dlopen %s\n", path);
            return h;
        }
    }
    return 0;
}

int main(void) {
    const char *subdir = conda_subdir();
    printf("host subdir %s\n", subdir);

    char depsdir[PMAX];
    if (extract_deps(subdir, depsdir, sizeof(depsdir)) != 0) {
        return 1;
    }

    void *h = open_zlib(depsdir);
    if (!h) {
        fprintf(stderr, "could not dlopen zlib from %s\n", depsdir);
        return 1;
    }
    const char *(*zlibVersion)(void) =
        (const char *(*)(void))cosmo_dltramp(cosmo_dlsym(h, "zlibVersion"));
    if (!zlibVersion) {
        fprintf(stderr, "zlibVersion missing\n");
        return 1;
    }
    printf("zlibVersion %s\n", zlibVersion());
    return 0;
}
