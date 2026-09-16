SPDLOG_SRC := https://github.com/gabime/spdlog/archive/refs/tags/v1.15.2.tar.gz

SPDLOG_CONFIG_ARGS = -DCMAKE_INSTALL_PREFIX=$$(COSMOS)\
    -DCMAKE_PREFIX_PATH=$$(COSMOS)\
    -DCMAKE_IGNORE_PREFIX_PATH="/usr/;/usr/include"\
    -DBUILD_SHARED_LIBS=OFF\
    -DCMAKE_BUILD_TYPE=Release\
    -DCMAKE_C_FLAGS="-Os"\
    -DCMAKE_CXX_FLAGS="-Os"\
    -DSPDLOG_FMT_EXTERNAL=ON\
    -DSPDLOG_BUILD_EXAMPLE=OFF\
    -DSPDLOG_BUILD_TESTS=OFF\
    -DSPDLOG_INSTALL=ON

$(eval $(call DOWNLOAD_SOURCE,lib/spdlog,$(SPDLOG_SRC)))
SPDLOG_DEPS := lib/fmt
$(eval $(call SPECIFY_DEPS,lib/spdlog,$(SPDLOG_DEPS)))
$(eval $(call CMAKE_BUILD,lib/spdlog,$(SPDLOG_CONFIG_ARGS),$(SPDLOG_CONFIG_ARGS)))

o/lib/spdlog/built.fat: FATTEN_COMMAND = $(DUMMYLINK0)
