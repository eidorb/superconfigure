#pragma once

#include <dlfcn.h>

/* Extract bundled conda-forge DSOs from /zip/linux-64 into <exe>/deps.
 * Then dlopen from that directory. Host X server / GPU still required. */
void deps_extract(void);
void deps_preload(void);
void *deps_dlopen(const char *soname, int flags);
const char *deps_dir(void);
