
# Node-Zipfile
      
  Bindings to [libzip](http://nih.at/libzip/libzip.html) for handling zipfiles in [node](http://nodejs.org).
  

## Depends

  node >= 0.2.4 (development headers)


## Installation

  Install libzip:
  
    $ hg clone http://hg.nih.at/libzip
    $ cd libzip
    $ cmake .
    $ make
    $ sudo make install
  
  Install node-zipfile:
  
    $ git clone git://github.com/springmeyer/node-zipfile.git
    $ cd node-zipfile
    $ ./configure
    $ make
    $ sudo make install
    $ ./test.js

  Make sure the node modules are on your path:
  
    export NODE_PATH=/usr/local/lib/node/


## License

  BSD, see LICENSE.txt