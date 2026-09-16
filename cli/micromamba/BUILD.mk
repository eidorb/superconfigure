
MICROMAMBA_SRC := https://github.com/mamba-org/mamba/archive/refs/tags/2.9.0.tar.gz

MICROMAMBA_DEPS := \
    lib/fmt lib/spdlog lib/json lib/expected lib/simdjson \
    lib/yaml-cpp lib/reproc lib/msgpack-c lib/nghttp2 lib/libsolv \
    lib/cli11 \
    compress/libarchive compress/xz compress/lz4 compress/brotli \
    lib/libexpat lib/pcre \
    web/curl lib/certs \
    cosmo-repo/base cosmo-repo/compress

$(eval $(call DOWNLOAD_SOURCE,cli/micromamba,$(MICROMAMBA_SRC)))
$(eval $(call SPECIFY_DEPS,cli/micromamba,$(MICROMAMBA_DEPS)))

o/cli/micromamba/configured.x86_64: CONFIG_COMMAND = $(BASELOC)/cli/micromamba/config-wrapper
o/cli/micromamba/configured.aarch64: CONFIG_COMMAND = $(BASELOC)/cli/micromamba/config-wrapper

o/cli/micromamba/built.x86_64: BUILD_COMMAND = ninja -j$(MAXPROC)
o/cli/micromamba/built.aarch64: BUILD_COMMAND = ninja -j$(MAXPROC)

o/cli/micromamba/installed.x86_64: INSTALL_COMMAND = ninja install -j$(MAXPROC)
o/cli/micromamba/installed.aarch64: INSTALL_COMMAND = ninja install -j$(MAXPROC)

o/cli/micromamba/built.fat: FATTEN_COMMAND = $(BASELOC)/config/apelink_folder.sh
o/cli/micromamba/built.fat: BINS = micromamba share/ssl

micromamba: o/cli/micromamba/built.fat
.PHONY: micromamba
