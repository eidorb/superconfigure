
GIT_CRYPT_SRC := https://github.com/AGWA/git-crypt/archive/refs/tags/0.8.0.tar.gz
GIT_CRYPT_DEPS := lib/openssl

$(eval $(call DOWNLOAD_SOURCE,web/git-crypt,$(GIT_CRYPT_SRC)))
$(eval $(call SPECIFY_DEPS,web/git-crypt,$(GIT_CRYPT_DEPS)))

# git-crypt uses a simple Makefile, not autotools
# We override config to be a no-op and do the build in build step

o/web/git-crypt/configured.x86_64: CONFIG_COMMAND = true
o/web/git-crypt/configured.aarch64: CONFIG_COMMAND = true

o/web/git-crypt/built.x86_64: BUILD_COMMAND = \
    cp -r $(BASELOC)/o/web/git-crypt/git-crypt*/* . && \
    make CXX="$(CXX)" \
         CXXFLAGS="-Wall -pedantic -Wno-long-long -O2 -std=c++11 -I$(COSMOS)/include" \
         LDFLAGS="-L$(COSMOS)/lib -lcrypto"

o/web/git-crypt/built.aarch64: BUILD_COMMAND = \
    cp -r $(BASELOC)/o/web/git-crypt/git-crypt*/* . && \
    make CXX="$(CXX)" \
         CXXFLAGS="-Wall -pedantic -Wno-long-long -O2 -std=c++11 -I$(COSMOS)/include" \
         LDFLAGS="-L$(COSMOS)/lib -lcrypto"

o/web/git-crypt/installed.x86_64: INSTALL_COMMAND = make install PREFIX=$(COSMOS)
o/web/git-crypt/installed.aarch64: INSTALL_COMMAND = make install PREFIX=$(COSMOS)

o/web/git-crypt/built.fat: BINS = git-crypt
