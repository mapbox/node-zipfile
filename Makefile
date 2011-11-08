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
	@NODE_PATH=.:./lib:$NODE_PATH ./node_modules/.bin/expresso

fix:
	@fixjsstyle lib/*js bin/*js test/*js

lint:
	@./node_modules/.bin/jshint lib/*js bin/*js test/*js

.PHONY: test lint fix