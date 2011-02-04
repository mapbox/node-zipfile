all: zipfile.node

install:
	cd deps/libzip-0.9.3; make; cd ../../
	node-waf -v build install

zipfile.node:
	cd deps/libzip-0.9.3; make; cd ../../
	node-waf -v build

clean:
	cd deps/libzip-0.9.3; make clean distclean; cd ../../
	node-waf -v clean distclean

uninstall:
	node-waf -v uninstall

test:
	node test.js