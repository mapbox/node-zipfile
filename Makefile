all: zipfile.node

zipfile.node:
	PATH=`npm explore npm -g -- pwd`/bin/node-gyp-bin:./node_modules/.bin:$${PATH} && ./node_modules/.bin/node-pre-gyp build

clean:
	rm -rf lib/binding/
	rm -rf test/tmp/*
	rm -rf ./build

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