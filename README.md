# node-zipfile
      
Bindings to [libzip](http://nih.at/libzip/libzip.html) for handling zipfile archives in [node](http://nodejs.org).


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

node-zipfile 0.3.x depends on node v0.6 and above

node-zipfile 0.1.x-0.2.x works with node v0.2-v0.4

libzip (optionally, otherwise will build against bundled version)


## Installation

You can install the latest tag via npm:

    npm install zipfile

Or install from github master:

    git clone git://github.com/springmeyer/node-zipfile.git
    cd node-zipfile
    ./configure
    make
    make install


## Dynamically linking against libzip

node-zipfile depends on libzip, but by default
bundles a copy in deps/ which is statically linked.

If you want to use an external libzip first install it:

Debian:

    sudo apt-get install libzip-dev libzip1

OS X:
  
    brew install libzip

Or from source:

    wget http://nih.at/libzip/libzip-0.10.tar.bz2
    tar xvf libzip-0.10.tar.bz2
    cd libzip-0.10
    ./configure
    make
    sudo make install

Then on linux do:

    sudo ldconfig

Now, configure node-zipfile with the --shared-libzip option:
 
    ./configure --shared-libzip

If you installed libzip in a custom location then configure like:
  
    ./configure --shared-libzip --libzip=/opt/local

Otherwise /usr/ and /usr/local will be searched for libzip:

    ./configure --shared-libzip
    make
    sudo make install
    ./test.js

## Building libzip with cmake on windows:

Note: needs cygwin so the .sh shell scripts work that libzip cmake files call.

    git clone git://github.com/springmeyer/node-zipfile.git
    cd node-zipfile/deps/
    bsdtar xvf libzip-0.10.tar.bz2
    cd libzip
    mkdir build_vc100
    cd build_vc100
    cmake -DZLIB_LIBRARY=c:\dev2\zlib\zlib.lib -DZLIB_INCLUDE_DIR=c:\dev2\zlib -G"NMake Makefiles" -DCMAKE_BUILD_TYPE=Release ..\

Then edit lib\zipconf.h removing 'inttypes.h' with:

    #include <stdint.h>
    #include <limits.h>

Finally build libzip

    nmake /f Makefile zip
    
Then compile node-zipfile:

    TODO
 

## License

  BSD, see LICENSE.txt