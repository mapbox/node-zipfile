
# Node-Zipfile
      
  Bindings to [libzip](http://nih.at/libzip/libzip.html) for handling zipfiles in [node](http://nodejs.org).


# Example

    > var zipfile = require('zipfile')
    > var zf = new zipfile.ZipFile('./data/world_merc.zip')
    > zf
    { names: [ 'world_merc.dbf', 'world_merc.prj', 'world_merc.shp', 'world_merc.shx' ],
      count: 4 }
    > var buffer = zf.readFileSync('world_merc.prj')
    > buffer.toString()
    'PROJCS["Google Maps Global Mercator",GEOGCS .... '


## Depends

  node >= 0.2.4 (development headers)
  
  libzip


## Installation

  Install libzip:
  
  Debian:
  
    $ sudo apt-get install libzip-dev libzip1
  
  Or from source:

    $ hg clone http://hg.nih.at/libzip
    $ cd libzip
    $ cmake .
    $ make
    $ sudo make install
    $ sudo ldconfig
  
  Install node-zipfile:
  
    $ git clone git://github.com/springmeyer/node-zipfile.git
    $ cd node-zipfile

  If you installed libzip in a custom location then configure like:
    
    $ configure --libzip=/opt/local

  Otherwise /usr/ and /usr/local will be searched for libzip:

    $ ./configure
    $ make
    $ sudo make install
    $ ./test.js

  
  Make sure the node modules are on your path:
  
    export NODE_PATH=/usr/local/lib/node/

  Or you can install via npm:
  
    $ npm install zipfile


## License

  BSD, see LICENSE.txt