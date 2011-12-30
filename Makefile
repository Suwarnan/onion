include Makefile.config

VERSION=$(shell grep define\ VERSION\  src/version.h | cut -f2 -d\")
REVISION=$(shell grep define\ REVISION\  src/version.h | grep -o "[0-9]\{1,\}")
PACKAGE_NAME=$(shell if [ $(VERSION) = "svn" ]; then echo "onion-$(VERSION)-r$(REVISION)"; else echo "onion-$(VERSION)"; fi)

all:
	cd src && make

install:
	cd src && make install
	mkdir -p $(INSTALL_DATA) && cp -r doc/ $(INSTALL_DATA)

clean:
	cd src && make clean

dist:
	rm -rf packages/$(PACKAGE_NAME)/ packages/$(PACKAGE_NAME).tar.gz
	mkdir -p packages/$(PACKAGE_NAME)/src/
	cp src/*.c src/*.h src/Makefile packages/$(PACKAGE_NAME)/src/
	cp -r Makefile Makefile.config COPYING README doc packages/$(PACKAGE_NAME)/
	cd packages && tar --exclude='.svn' -czvf $(PACKAGE_NAME).tar.gz $(PACKAGE_NAME)/
