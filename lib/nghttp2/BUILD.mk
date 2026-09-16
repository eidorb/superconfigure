
NGHTTP2_SRC := https://github.com/nghttp2/nghttp2/releases/download/v1.65.0/nghttp2-1.65.0.tar.gz

NGHTTP2_CONFIG_ARGS = --prefix=$$(COSMOS)\
    --enable-static --disable-shared\
    --enable-lib-only --disable-examples --disable-app\
    --disable-python-bindings --disable-failmalloc\
    --without-libxml2 --without-jansson --without-jemalloc\
    --without-libevent-openssl --without-libcares\
    --without-openssl --without-libbpf --without-libngtcp2\
    --without-libnghttp3 --without-systemd\
    --disable-http3 --disable-rpath --without-pic\
    CFLAGS="-Os"

$(eval $(call DOWNLOAD_SOURCE,lib/nghttp2,$(NGHTTP2_SRC)))
$(eval $(call AUTOTOOLS_BUILD,lib/nghttp2,$(NGHTTP2_CONFIG_ARGS),$(NGHTTP2_CONFIG_ARGS)))

o/lib/nghttp2/built.fat: FATTEN_COMMAND = $(DUMMYLINK0)
