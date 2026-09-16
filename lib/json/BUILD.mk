JSON_SRC := https://github.com/nlohmann/json/archive/refs/tags/v3.12.0.tar.gz

JSON_CONFIG_ARGS = -DCMAKE_INSTALL_PREFIX=$$(COSMOS)\
    -DCMAKE_PREFIX_PATH=$$(COSMOS)\
    -DCMAKE_IGNORE_PREFIX_PATH="/usr/;/usr/include"\
    -DBUILD_SHARED_LIBS=OFF\
    -DCMAKE_BUILD_TYPE=Release\
    -DCMAKE_C_FLAGS="-Os"\
    -DCMAKE_CXX_FLAGS="-Os"\
    -DJSON_BuildTests=OFF\
    -DJSON_Install=ON

$(eval $(call DOWNLOAD_SOURCE,lib/json,$(JSON_SRC)))
$(eval $(call CMAKE_BUILD,lib/json,$(JSON_CONFIG_ARGS),$(JSON_CONFIG_ARGS)))

o/lib/json/built.fat: FATTEN_COMMAND = $(DUMMYLINK0)
