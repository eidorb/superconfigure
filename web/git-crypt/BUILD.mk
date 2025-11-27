
GIT_CRYPT_SRC := https://github.com/AGWA/git-crypt/archive/refs/tags/0.8.0.tar.gz

GIT_CRYPT_DEPS := lib/openssl

$(eval $(call DOWNLOAD_SOURCE,web/git-crypt,$(GIT_CRYPT_SRC)))
$(eval $(call SPECIFY_DEPS,web/git-crypt,$(GIT_CRYPT_DEPS)))

o/web/git-crypt/configured.x86_64: CONFIG_COMMAND = $(BASELOC)/web/git-crypt/config-wrapper
o/web/git-crypt/configured.aarch64: CONFIG_COMMAND = $(BASELOC)/web/git-crypt/config-wrapper

o/web/git-crypt/built.x86_64: BUILD_COMMAND = $(DUMMYLINK0)
o/web/git-crypt/built.aarch64: BUILD_COMMAND = $(DUMMYLINK0)
o/web/git-crypt/installed.x86_64: INSTALL_COMMAND = $(DUMMYLINK0)
o/web/git-crypt/installed.aarch64: INSTALL_COMMAND = $(DUMMYLINK0)

o/web/git-crypt/built.fat: FATTEN_COMMAND = $(BASELOC)/config/apelink_folder.sh
o/web/git-crypt/built.fat: BINS = git-crypt
