
# Node-Zipfile
      
  Bindings to [libzip](http://nih.at/libzip/libzip.html) for handling zipfiles in [node](http://nodejs.org).


# Example

    > var zipfile = require('zipfile')
    > var fs = require('fs')
    > // Open:
    > var zf = new zipfile.ZipFile('./data/world_merc.zip')
    > zf
    { names: [ 'world_merc.dbf', 'world_merc.prj', 'world_merc.shp', 'world_merc.shx' ],
      count: 4 }
    > // Synchronous read:
    > var buffer = zf.readFileSync('world_merc.prj')
    > buffer.toString()
    'PROJCS["Google Maps Global Mercator",GEOGCS .... '
    > // Asynchronous read:
    > var stream = zf.createReadStream('world_merc.prj')
    > var fstream = fs.createWriteStream('/tmp/world_merc.prj')
    > stream.pipe(fstream)
    > // Write:
    > zf.addDirectory('foo')
    > zf.addFile('foo/world_merc.prj', '/tmp/world_merc.prj')
    > zf.save(function (err) {
    ... if (err)
    ...   console.log("Error :" + err)
    ... else
    ...   console.log("Done")
    ... })
    > Done
    > zf.replaceFile('foo/world_merc.prj', '/tmp/world_merc.prj')
    > zf.save(...)


# Note:

* libzip is not thread-safe. Thus, there can only be one of the following operation at a given time:
    * `zf.createReadStream`
    * `zf.readFileSync`
    * `zf.addFile`
    * `zf.replaceFile`
    * `zf.save`
  Call to `zf.readFileSync`, `zf.addFile, `zf.replaceFile` are synchronous. Upon opening a read stream,
  the zip archive can be used with another of the operations above one the `'close'` event has been emitted
  by the stream.
* Adding and replacing files in the archive are synchronous and return immediatly. Files are actually written
  when calling `zf.save`, which is asynchronous. 

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
