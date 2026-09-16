EXPECTED_SRC := https://github.com/TartanLlama/expected/archive/refs/tags/v1.1.0.tar.gz

EXPECTED_CONFIG_ARGS = -DCMAKE_INSTALL_PREFIX=$$(COSMOS)\
    -DCMAKE_PREFIX_PATH=$$(COSMOS)\
    -DCMAKE_IGNORE_PREFIX_PATH="/usr/;/usr/include"\
    -DBUILD_SHARED_LIBS=OFF\
    -DCMAKE_BUILD_TYPE=Release\
    -DCMAKE_C_FLAGS="-Os"\
    -DCMAKE_CXX_FLAGS="-Os"\
    -DEXPECTED_BUILD_TESTS=OFF\
    -DEXPECTED_BUILD_PACKAGE=ON

$(eval $(call DOWNLOAD_SOURCE,lib/expected,$(EXPECTED_SRC)))
$(eval $(call CMAKE_BUILD,lib/expected,$(EXPECTED_CONFIG_ARGS),$(EXPECTED_CONFIG_ARGS)))

o/lib/expected/built.fat: FATTEN_COMMAND = $(DUMMYLINK0)
