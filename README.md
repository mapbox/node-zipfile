# node-zipfile
      
Bindings to [libzip](http://nih.at/libzip/libzip.html) for handling zipfile archives in [node](http://nodejs.org).

[![Build Status](https://secure.travis-ci.org/mapbox/node-zipfile.png)](http://travis-ci.org/mapbox/node-zipfile)

## Example

    var zipfile = require('zipfile');
    var zf = new zipfile.ZipFile('./test/data/world_merc.zip');
    zf
    { names: [ 'world_merc.dbf', 'world_merc.prj', 'world_merc.shp', 'world_merc.shx' ],
      count: 4 }
    var buffer = zf.readFileSync('world_merc.prj');
    buffer.toString()
    'PROJCS["Google Maps Global Mercator",GEOGCS .... '

## Depends

 - Node v0.10.x or v0.8.x

## Installation

Install from binary:

    npm install

Install from source:

    npm install --build-from-source

## Dynamically linking against libzip

`node-zipfile` depends on libzip, but by default
bundles a copy in deps/ which is statically linked and packaged as a binary.

If you want to use an external libzip first install it:

Debian:

    sudo apt-get install libzip-dev libzip1

OS X:
  
    brew install libzip

Then configure node-zipfile with the --shared_libzip option:
 
    npm install  --build-from-source --shared_libzip

If you installed libzip in a custom location then configure like:
  
    npm install  --build-from-source --shared_libzip --shared_libzip_includes=/opt/local/include --shared_libzip_libpath=/opt/local/lib

## License

  BSD, see LICENSE.txt