all: zipfile.node

NPROCS:=1
OS:=$(shell uname -s)

ifeq ($(OS),Linux)
	NPROCS:=$(shell grep -c ^processor /proc/cpuinfo)
endif
ifeq ($(OS),Darwin)
	NPROCS:=$(shell sysctl -n hw.ncpu)
endif

install:
	node-waf -v build install

zipfile.node:
	node-waf -v build

clean:
	node-waf -v clean distclean

uninstall:
	node-waf -v uninstall

test:
	@PATH=./node_modules/mocha/bin:${PATH} && NODE_PATH=./lib:$NODE_PATH mocha -R spec

fix:
	@fixjsstyle lib/*js bin/*js test/*js

fixc:
	@tools/fix_cpp_style.sh

lint:
	@./node_modules/.bin/jshint lib/*js bin/*js test/*js

lintc:
	@cpplint.py --verbose=3 --filter=-legal,-build/namespaces,-whitespace/line_length src/*.* include/zipfile/*.* 

gyp:
	rm -rf ./projects/makefiles/
	python gyp/gyp build.gyp --depth=. -f make --generator-output=./projects/makefiles
	#./build.py
	make -j$(NPROCS) -C ./projects/makefiles/ V=1
	cp projects/makefiles/out/Default/_zipfile.node lib/_zipfile.node

.PHONY: test lint fix lintc fixc gyp