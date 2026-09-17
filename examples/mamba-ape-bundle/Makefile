COSMOCC ?= /workspace/superconfigure/cosmopolitan/.cosmocc/3.9.2/bin/cosmocc
ZIPOBJ ?= /workspace/superconfigure/cosmopolitan/.cosmocc/3.9.2/bin/zipobj
MAMBA ?= /workspace/superconfigure/results/bin/micromamba.com
PYTHON ?= python3
SUBDIRS ?= linux-64 linux-aarch64 osx-64 osx-arm64 win-64 win-arm64

.PHONY: all bundle program test clean

all: program

bundle: vendor/.stamp

vendor/.stamp: environment.yml scripts/bundle.py
	MAMBA=$(MAMBA) $(PYTHON) scripts/bundle.py
	touch $@

program: bin/hello.com

.build/payload.zip.o: vendor/.stamp
	mkdir -p .build .build/.aarch64
	cd vendor && $(ZIPOBJ) -a x86_64 -o ../.build/payload.zip.o \
		$$(find $(SUBDIRS) -type f)
	cd vendor && $(ZIPOBJ) -a aarch64 -o ../.build/.aarch64/payload.zip.o \
		$$(find $(SUBDIRS) -type f)

bin/hello.com: src/hello.c .build/payload.zip.o
	mkdir -p bin
	$(COSMOCC) -mcosmo -mtiny -O2 -Wall -Wno-format-truncation \
		-o $@ src/hello.c .build/payload.zip.o

test: bin/hello.com
	./bin/hello.com

clean:
	rm -rf bin vendor .build
