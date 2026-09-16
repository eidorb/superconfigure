MSGPACK_C_SRC := https://github.com/msgpack/msgpack-c/archive/refs/tags/c-6.1.0.tar.gz

MSGPACK_C_CONFIG_ARGS = -DCMAKE_INSTALL_PREFIX=$$(COSMOS)\
    -DCMAKE_PREFIX_PATH=$$(COSMOS)\
    -DCMAKE_IGNORE_PREFIX_PATH="/usr/;/usr/include"\
    -DBUILD_SHARED_LIBS=OFF\
    -DCMAKE_BUILD_TYPE=Release\
    -DCMAKE_C_FLAGS="-Os"\
    -DCMAKE_CXX_FLAGS="-Os"\
    -DMSGPACK_BUILD_EXAMPLES=OFF\
    -DMSGPACK_BUILD_TESTS=OFF\
    -DMSGPACK_ENABLE_SHARED=OFF\
    -DMSGPACK_ENABLE_STATIC=ON

$(eval $(call DOWNLOAD_SOURCE,lib/msgpack-c,$(MSGPACK_C_SRC)))
$(eval $(call CMAKE_BUILD,lib/msgpack-c,$(MSGPACK_C_CONFIG_ARGS),$(MSGPACK_C_CONFIG_ARGS)))

o/lib/msgpack-c/built.fat: FATTEN_COMMAND = $(DUMMYLINK0)
