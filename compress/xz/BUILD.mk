
# Debian 13 (trixie/stable) xz-utils 5.8.1 orig tarball.
# Same sha256 as tukaani.org and Debian pool; GitHub Releases host.
XZ_SRC := https://github.com/tukaani-project/xz/releases/download/v5.8.1/xz-5.8.1.tar.xz

$(eval $(call DOWNLOAD_SOURCE,compress/xz,$(XZ_SRC)))

XZ_CONFIG_ARGS =  --prefix=$$(COSMOS)\
    --enable-shared=no --enable-static=yes\
    --disable-nls --disable-rpath\
    --without-pic --with-gnu-ld\
    --enable-sandbox=no\
    --sysconfdir=$$(COSMOS) --datarootdir=$$(COSMOS)/share\
    CFLAGS="-Os -std=gnu99"

$(eval $(call AUTOTOOLS_BUILD,compress/xz,$(XZ_CONFIG_ARGS),$(XZ_CONFIG_ARGS)))

o/compress/xz/built.fat: BINS = xz
