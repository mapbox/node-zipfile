all: node_zipfile.node

node_zipfile.node:
	`npm explore npm -g -- pwd`/bin/node-gyp-bin/node-gyp build

clean:
	@rm -rf ./build
	rm -f lib/node_zipfile.node
	rm -f test/tmp/*
	rm -rf ./deps/libzip-0.10/
	rm -rf ./build
	rm -rf ./out

rebuild:
	@make clean
	@./configure
	@make

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

.PHONY: test lint fix lintc fixc