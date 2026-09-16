REPROC_SRC := https://github.com/DaanDeMeyer/reproc/archive/refs/tags/v14.2.5.tar.gz

REPROC_CONFIG_ARGS = -DCMAKE_INSTALL_PREFIX=$$(COSMOS)\
    -DCMAKE_PREFIX_PATH=$$(COSMOS)\
    -DCMAKE_IGNORE_PREFIX_PATH="/usr/;/usr/include"\
    -DBUILD_SHARED_LIBS=OFF\
    -DCMAKE_BUILD_TYPE=Release\
    -DCMAKE_C_FLAGS="-Os"\
    -DCMAKE_CXX_FLAGS="-Os"\
    -DREPROC++=ON\
    -DREPROC_TEST=OFF\
    -DREPROC_EXAMPLES=OFF\
    -DREPROC_DEVELOP=OFF

$(eval $(call DOWNLOAD_SOURCE,lib/reproc,$(REPROC_SRC)))
$(eval $(call CMAKE_BUILD,lib/reproc,$(REPROC_CONFIG_ARGS),$(REPROC_CONFIG_ARGS)))

o/lib/reproc/built.fat: FATTEN_COMMAND = $(DUMMYLINK0)
