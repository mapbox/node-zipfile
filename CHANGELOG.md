# changelog

## v0.5.1

 - Fixed Solaris build
 - Fixed potential segfault when an invalid filename is requested from `zipfile.ReadFile` (#46)

## v0.5.0

 - Now supporting binary deploy through node-pre-gyp (#42). Run `npm install --build-from-source` now to build from source instead of installing from a binary.

## v0.4.3

 - Upgraded libzip to hg at http://hg.nih.at/libzip/rev/66d608d38359 to fix #36