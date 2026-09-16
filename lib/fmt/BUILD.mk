FMT_SRC := https://github.com/fmtlib/fmt/archive/refs/tags/11.1.4.tar.gz

FMT_CONFIG_ARGS = -DCMAKE_INSTALL_PREFIX=$$(COSMOS)\
    -DCMAKE_PREFIX_PATH=$$(COSMOS)\
    -DCMAKE_IGNORE_PREFIX_PATH="/usr/;/usr/include"\
    -DBUILD_SHARED_LIBS=OFF\
    -DCMAKE_BUILD_TYPE=Release\
    -DCMAKE_C_FLAGS="-Os"\
    -DCMAKE_CXX_FLAGS="-Os"\
    -DFMT_DOC=OFF\
    -DFMT_TEST=OFF\
    -DFMT_OS=OFF\
    -DFMT_INSTALL=ON

$(eval $(call DOWNLOAD_SOURCE,lib/fmt,$(FMT_SRC)))
$(eval $(call CMAKE_BUILD,lib/fmt,$(FMT_CONFIG_ARGS),$(FMT_CONFIG_ARGS)))

o/lib/fmt/built.fat: FATTEN_COMMAND = $(DUMMYLINK0)
