#ifndef _COSMO_SOURCE
#define _COSMO_SOURCE
#endif
#include "deps.h"

#include <cosmo.h>
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#define PMAX 4096

static char g_exe_dir[PMAX];
static char g_deps_dir[PMAX];

static void dirname_copy(const char *path, char *out, size_t n) {
    snprintf(out, n, "%s", path ? path : ".");
    char *slash = strrchr(out, '/');
    char *bslash = strrchr(out, '\\');
    if (bslash && (!slash || bslash > slash)) {
        slash = bslash;
    }
    if (!slash) {
        snprintf(out, n, ".");
        return;
    }
    *slash = 0;
    if (!out[0]) {
        snprintf(out, n, "/");
    }
}

const char *deps_exe_dir(void) {
    if (g_exe_dir[0]) {
        return g_exe_dir;
    }
    const char *exe = GetProgramExecutableName();
    dirname_copy(exe && exe[0] ? exe : ".", g_exe_dir, sizeof(g_exe_dir));
    return g_exe_dir;
}

const char *deps_dir(void) {
    if (g_deps_dir[0]) {
        return g_deps_dir;
    }
    snprintf(g_deps_dir, sizeof(g_deps_dir), "%s/deps", deps_exe_dir());
    return g_deps_dir;
}

static int mkdir_p(const char *path) {
    if (mkdir(path, 0755) == 0 || errno == EEXIST) {
        return 0;
    }
    return -1;
}

static int copy_file(const char *src, const char *dst) {
    int in = open(src, O_RDONLY);
    if (in < 0) {
        return -1;
    }
    int out = open(dst, O_WRONLY | O_CREAT | O_TRUNC, 0755);
    if (out < 0) {
        close(in);
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
    return n < 0 ? -1 : 0;
}

static void make_alias(const char *libdir, const char *from, const char *to) {
    char src[PMAX], dst[PMAX];
    snprintf(src, sizeof(src), "%s/%s", libdir, from);
    snprintf(dst, sizeof(dst), "%s/%s", libdir, to);
    if (access(dst, F_OK) == 0 || access(src, F_OK) != 0) {
        return;
    }
    copy_file(src, dst);
}

static void alias_sonames(const char *libdir) {
    DIR *d = opendir(libdir);
    if (!d) {
        return;
    }
    struct dirent *e;
    while ((e = readdir(d))) {
        const char *n = e->d_name;
        const char *so = strstr(n, ".so.");
        if (!so) {
            continue;
        }
        char base[128];
        size_t stem = (size_t)(so - n);
        if (stem + 4 >= sizeof(base)) {
            continue;
        }
        memcpy(base, n, stem);
        memcpy(base + stem, ".so", 4);
        make_alias(libdir, n, base);
        char major[136];
        if (so[4]) {
            snprintf(major, sizeof(major), "%s.%c", base, so[4]);
            if (strcmp(major, n) != 0) {
                make_alias(libdir, n, major);
            }
        }
    }
    closedir(d);
    make_alias(libdir, "libGL.so", "libgl.so");
}

static int extract_from(const char *srcroot, const char *libdst) {
    DIR *d = opendir(srcroot);
    if (!d) {
        return -1;
    }
    int nfiles = 0;
    struct dirent *e;
    while ((e = readdir(d))) {
        if (e->d_name[0] == '.') {
            continue;
        }
        char from[PMAX], to[PMAX];
        snprintf(from, sizeof(from), "%s/%s", srcroot, e->d_name);
        snprintf(to, sizeof(to), "%s/%s", libdst, e->d_name);
        if (copy_file(from, to) == 0) {
            nfiles++;
        }
    }
    closedir(d);
    return nfiles > 0 ? 0 : -1;
}

void deps_extract(void) {
    if (!IsLinux()) {
        return;
    }
    char libdst[PMAX];
    snprintf(libdst, sizeof(libdst), "%s/lib", deps_dir());
    char marker[PMAX + 16];
    snprintf(marker, sizeof(marker), "%s/libX11.so.6", libdst);
    if (access(marker, F_OK) == 0) {
        return;
    }

    mkdir_p(deps_dir());
    mkdir_p(libdst);

    if (extract_from("/zip/linux-64/lib", libdst) != 0) {
        extract_from("vendor/linux-64/lib", libdst);
    }
    alias_sonames(libdst);
}

void *deps_dlopen(const char *soname, int flags) {
    char path[PMAX];
    snprintf(path, sizeof(path), "%s/lib/%s", deps_dir(), soname);
    void *h = cosmo_dlopen(path, flags);
    if (h) {
        return h;
    }
    return cosmo_dlopen((char *)soname, flags);
}

void deps_preload(void) {
    if (!IsLinux()) {
        return;
    }
    static const char *order[] = {
        "libXau.so",
        "libXdmcp.so",
        "libxcb.so",
        "libX11.so",
        "libXext.so",
        "libXrender.so",
        "libXfixes.so",
        "libXcursor.so",
        "libXi.so",
        "libGLdispatch.so",
        "libGLX.so",
        "libGL.so",
        NULL,
    };
    for (int i = 0; order[i]; i++) {
        (void)deps_dlopen(order[i], RTLD_NOW | RTLD_GLOBAL);
    }
}
