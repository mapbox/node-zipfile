all: zipfile.node

install:
	node-waf -v build install

zipfile.node:
	node-waf -v build

clean:
	node-waf -v clean distclean

uninstall:
	node-waf -v uninstall

test:
	@NODE_PATH=.:./lib:$NODE_PATH mocha -r zipfile -r assert

fix:
	@fixjsstyle lib/*js bin/*js test/*js

fixc:
	@tools/fix_cpp_style.sh

lint:
	@./node_modules/.bin/jshint lib/*js bin/*js test/*js

lintc:
	@cpplint.py --verbose=3 --filter=-legal,-build/namespaces,-whitespace/line_length src/*.* include/zipfile/*.* 

.PHONY: test lint fix lintc fixc