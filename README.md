
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
  
  libzip (optionally)


## Installation

  You can install the latest tag via npm:
  
    $ npm install zipfile
  
  Or install from github master:
  
    $ git clone git://github.com/springmeyer/node-zipfile.git
    $ cd node-zipfile
    $ ./configure
    $ make
    $ make install


## Dynamically linking against libzip

  node-zipfile depends on libzip, but by default
  bundles a copy in deps/ which is statically linked.
  
  If you want to use an external libzip first install it:
  
  Debian:
  
    $ sudo apt-get install libzip-dev libzip1
  
  OS X:
    
    $ brew install libzip
  
  Or from source:

    $ wget http://nih.at/libzip/libzip-0.10.tar.bz2
    $ tar xvf libzip-0.10.tar.bz2
    $ cd libzip-0.10
    $ ./configure
    $ make
    $ sudo make install
  
  Then on linux do:
  
    $ sudo ldconfig

  Now, configure node-zipfile with the --shared-libzip option:
   
    $ ./configure --shared-libzip
  
  If you installed libzip in a custom location then configure like:
    
    $ ./configure --shared-libzip --libzip=/opt/local

  Otherwise /usr/ and /usr/local will be searched for libzip:

    $ ./configure --shared-libzip
    $ make
    $ sudo make install
    $ ./test.js


## License

  BSD, see LICENSE.txt