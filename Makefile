-include config.mk
-include installdeps.mk

CC       = gcc
CXX      = g++
CFLAGS   += -g -rdynamic -Wall -O2
LDFLAGS  += $(LIBS) -lpthread -lz
BUILD_DIR = build

BINARY = slow5tools
OBJ = $(BUILD_DIR)/main.o \
      $(BUILD_DIR)/f2s.o \
      $(BUILD_DIR)/s2f.o \
      $(BUILD_DIR)/index.o \
      $(BUILD_DIR)/extract.o \
	  $(BUILD_DIR)/slow5idx.o \
	  $(BUILD_DIR)/kstring.o \
	  $(BUILD_DIR)/misc.o \
	  $(BUILD_DIR)/thread.o \
	  $(BUILD_DIR)/read_fast5.o \


PREFIX = /usr/local
VERSION = `git describe --tags`

.PHONY: clean distclean format test install uninstall

$(BINARY): src/config.h $(HDF5_LIB) $(OBJ)
	$(CXX) $(CFLAGS) $(OBJ) $(LDFLAGS) -o $@

$(BUILD_DIR)/main.o: src/main.c src/slow5misc.h src/error.h
	$(CXX) $(CFLAGS) $(CPPFLAGS) $< -c -o $@

$(BUILD_DIR)/f2s.o: src/f2s.c src/slow5.h src/error.h
	$(CXX) $(CFLAGS) $(CPPFLAGS) $< -c -o $@

$(BUILD_DIR)/s2f.o: src/s2f.c src/slow5.h src/error.h
	$(CXX) $(CFLAGS) $(CPPFLAGS) $< -c -o $@

$(BUILD_DIR)/index.o: src/index.c src/slow5.h src/error.h
	$(CXX) $(CFLAGS) $(CPPFLAGS) $< -c -o $@

$(BUILD_DIR)/extract.o: src/extract.c src/slow5.h src/error.h
	$(CXX) $(CFLAGS) $(CPPFLAGS) $< -c -o $@

#$(BUILD_DIR)/fastt_main.o: src/fastt_main.c src/slow5.h src/fast5lite.h src/slow5misc.h src/error.h
#	$(CXX) $(CFLAGS) $(CPPFLAGS) $< -c -o $@

$(BUILD_DIR)/slow5idx.o: src/slow5idx.c src/slow5idx.h
	$(CXX) $(CFLAGS) $(CPPFLAGS) $< -c -o $@

$(BUILD_DIR)/kstring.o: src/kstring.c src/kstring.h
	$(CXX) $(CFLAGS) $(CPPFLAGS) $< -c -o $@

$(BUILD_DIR)/misc.o: src/misc.c
	$(CXX) $(CFLAGS) $(CPPFLAGS) $< -c -o $@

$(BUILD_DIR)/thread.o: src/thread.c src/slow5idx.h
	$(CXX) $(CFLAGS) $(CPPFLAGS) $< -c -o $@

$(BUILD_DIR)/read_fast5.o: src/read_fast5.c src/slow5idx.h
	$(CXX) $(CFLAGS) $(CPPFLAGS) $< -c -o $@


src/config.h:
	echo "/* Default config.h generated by Makefile */" >> $@
	echo "#define HAVE_HDF5_H 1" >> $@

$(BUILD_DIR)/lib/libhdf5.a:
	if command -v curl; then \
		curl -o $(BUILD_DIR)/hdf5.tar.bz2 https://support.hdfgroup.org/ftp/HDF5/releases/hdf5-$(HDF5_MAJOR_MINOR)/hdf5-$(HDF5_VERSION)/src/hdf5-$(HDF5_VERSION).tar.bz2; \
	else \
		wget -O $(BUILD_DIR)/hdf5.tar.bz2 https://support.hdfgroup.org/ftp/HDF5/releases/hdf5-$(HDF5_MAJOR_MINOR)/hdf5-$(HDF5_VERSION)/src/hdf5-$(HDF5_VERSION).tar.bz2; \
	fi
	tar -xf $(BUILD_DIR)/hdf5.tar.bz2 -C $(BUILD_DIR)
	mv $(BUILD_DIR)/hdf5-$(HDF5_VERSION) $(BUILD_DIR)/hdf5
	rm -f $(BUILD_DIR)/hdf5.tar.bz2
	cd $(BUILD_DIR)/hdf5 && \
	./configure --prefix=`pwd`/../ && \
	make -j8 && \
	make install

clean:
	rm -rf $(BINARY) $(BUILD_DIR)/*.o

# Delete all gitignored files (but not directories)
distclean: clean
	git clean -f -X
	rm -rf $(BUILD_DIR)/* autom4te.cache

dist: distclean
	mkdir -p slow5tools-$(VERSION)
	autoreconf
	cp -r README.md LICENSE Makefile configure.ac config.mk.in \
		installdeps.mk src docs build configure slow5tools-$(VERSION)
	mkdir -p slow5tools-$(VERSION)/scripts
	cp scripts/install-hdf5.sh slow5tools-$(VERSION)/scripts
	tar -zcf slow5tools-$(VERSION)-release.tar.gz slow5tools-$(VERSION)
	rm -rf slow5tools-$(VERSION)

binary:
	mkdir -p slow5tools-$(VERSION)
	make clean
	make && mv slow5tools slow5tools-$(VERSION)/slow5tools_x86_64_linux
	cp -r README.md LICENSE docs slow5tools-$(VERSION)/
	#mkdir -p slow5tools-$(VERSION)/scripts
	#cp scripts/test.sh slow5tools-$(VERSION)/scripts
	tar -zcf slow5tools-$(VERSION)-binaries.tar.gz slow5tools-$(VERSION)
	rm -rf slow5tools-$(VERSION)

install: $(BINARY)
	mkdir -p $(DESTDIR)$(PREFIX)/bin
	mkdir -p $(DESTDIR)$(PREFIX)/share/man/man1
	cp -f $(BINARY) $(DESTDIR)$(PREFIX)/bin
	gzip < docs/slow5tools.1 > $(DESTDIR)$(PREFIX)/share/man/man1/slow5tools.1.gz

uninstall:
	rm -f $(DESTDIR)$(PREFIX)/bin/$(BINARY) \
		$(DESTDIR)$(PREFIX)/share/man/man1/slow5tools.1.gz

test: $(BINARY)
	./test/test.sh
