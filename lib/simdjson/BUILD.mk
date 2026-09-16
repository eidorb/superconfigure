SIMDJSON_SRC := https://github.com/simdjson/simdjson/archive/refs/tags/v3.12.3.tar.gz

SIMDJSON_CONFIG_ARGS = -DCMAKE_INSTALL_PREFIX=$$(COSMOS)\
    -DCMAKE_PREFIX_PATH=$$(COSMOS)\
    -DCMAKE_IGNORE_PREFIX_PATH="/usr/;/usr/include"\
    -DBUILD_SHARED_LIBS=OFF\
    -DCMAKE_BUILD_TYPE=Release\
    -DCMAKE_C_FLAGS="-Os"\
    -DCMAKE_CXX_FLAGS="-Os"\
    -DSIMDJSON_BUILD_STATIC_LIB=ON\
    -DSIMDJSON_DEVELOPER_MODE=OFF\
    -DSIMDJSON_ENABLE_THREADS=ON

# cosmocc is a cross compiler; CMake otherwise inherits the host CPU
# and simdjson then injects x86-only -mno-avx256-* flags on aarch64.
SIMDJSON_CONFIG_ARGS_X86 = $(SIMDJSON_CONFIG_ARGS) -DCMAKE_SYSTEM_PROCESSOR=x86_64
SIMDJSON_CONFIG_ARGS_ARM = $(SIMDJSON_CONFIG_ARGS) -DCMAKE_SYSTEM_PROCESSOR=aarch64

$(eval $(call DOWNLOAD_SOURCE,lib/simdjson,$(SIMDJSON_SRC)))
$(eval $(call CMAKE_BUILD,lib/simdjson,$(SIMDJSON_CONFIG_ARGS_X86),$(SIMDJSON_CONFIG_ARGS_ARM)))

o/lib/simdjson/built.fat: FATTEN_COMMAND = $(DUMMYLINK0)
