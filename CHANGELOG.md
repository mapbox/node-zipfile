# changelog

## v0.5.12

- Added node v8 and v10 support

## v0.5.11

 - Republished with `mkdirp` inside node_modules/node-pre-gyp/node_modules/

## v0.5.10

 - Added support for node v6, v7
 - Upgraded to nan@2.4.x
 - Fixed resource leak (#69)

## v0.5.9

 - Upgraded to zlib 1.2.8

## v0.5.8

 - Updated to work with NAN 2 and Node.js 4/5 (@elaberge)

## v0.5.7

 - Updated node-pre-gyp to v0.6.2
 - Support for io.js v1.0.2

## v0.5.6

 - Support for building against io.js v1.0.1

## v0.5.5

 - Added copyFile method for efficient handling of large zip archive members.

## v0.5.4

 - Now supporting node v.11.14 and Visual Studio 2014 (--toolset=v140)

## v0.5.3

 - Now supporting node v.11.x

## v0.5.2

 - Fixed bundling of node-pre-gyp

## v0.5.1

 - Fixed Solaris build
 - Fixed potential segfault when an invalid filename is requested from `zipfile.ReadFile` (#46)

## v0.5.0

 - Now supporting binary deploy through node-pre-gyp (#42). Run `npm install --build-from-source` now to build from source instead of installing from a binary.

## v0.4.3

 - Upgraded libzip to hg at http://hg.nih.at/libzip/rev/66d608d38359 to fix #36
