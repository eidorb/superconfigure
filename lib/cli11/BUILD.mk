CLI11_SRC := https://github.com/CLIUtils/CLI11/archive/refs/tags/v2.5.0.tar.gz

$(eval $(call DOWNLOAD_SOURCE,lib/cli11,$(CLI11_SRC)))

# Tarball extracts as CLI11-*, not cli11-* (CMAKE_BUILD glob would miss it).
o/lib/cli11/configured.x86_64: CONFIG_COMMAND = $(BASELOC)/lib/cli11/config-wrapper
o/lib/cli11/configured.aarch64: CONFIG_COMMAND = $(BASELOC)/lib/cli11/config-wrapper
o/lib/cli11/built.x86_64: BUILD_COMMAND = ninja -j$(MAXPROC)
o/lib/cli11/built.aarch64: BUILD_COMMAND = ninja -j$(MAXPROC)
o/lib/cli11/installed.x86_64: INSTALL_COMMAND = ninja install -j$(MAXPROC)
o/lib/cli11/installed.aarch64: INSTALL_COMMAND = ninja install -j$(MAXPROC)

o/lib/cli11/built.fat: FATTEN_COMMAND = $(DUMMYLINK0)
