# node-zipfile
      
Bindings to [libzip](http://nih.at/libzip/libzip.html) for handling zipfile archives in [node](http://nodejs.org).

[![Build Status](https://secure.travis-ci.org/springmeyer/node-zipfile.png)](http://travis-ci.org/springmeyer/node-zipfile)

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

 - Node >= 0.6.13
 - optionally depends on external libzip (by default builds against bundled version)

## Installation

You can install the latest tag via npm:

    npm install zipfile

Or install from github master:

    git clone git://github.com/springmeyer/node-zipfile.git
    cd node-zipfile
    npm install

## Dynamically linking against libzip

`node-zipfile` depends on libzip, but by default
bundles a copy in deps/ which is statically linked.

If you want to use an external libzip first install it:

Debian:

    sudo apt-get install libzip-dev libzip1

OS X:
  
    brew install libzip

Then configure node-zipfile with the --shared_libzip option:
 
    npm install --shared_libzip

If you installed libzip in a custom location then configure like:
  
    npm install --shared_libzip --shared_libzip_includes=/opt/local/include --shared_libzip_libpath=/opt/local/lib

## License

  BSD, see LICENSE.txt