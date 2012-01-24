del build.sln
rd /q /s Default
del lib\\_zipfile.node
python gyp/gyp build.gyp --depth=. -f msvs -G msvs_version=2010
msbuild build.sln
copy Default\\_zipfile.node lib\\_zipfile.node
rem test!
set NODE_PATH=lib
rem have to put zip.lib on PATH
set PATH=%PATH%;c:\\dev2\node-zipfile\\lib
copy deps\\libzip-0.10\\build_vc100\\lib\\zip.dll lib\\zip.dll
rem node node_modules\expresso\bin\expresso
mocha
rem node -e "console.log(require('zipfile'))"
